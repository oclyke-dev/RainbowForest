/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import * as WebSocket from 'ws';
import * as url from 'url';

import { debug } from '../../common/utility';
import { truth } from '../../common/secrets';
import staff, { Staff } from '../../common/staff';
import Message, {
  messageToString,
  strAsMessage,
  EntryData,
} from '../../common/message';

const private_wss = new WebSocket.Server({ noServer: true });
const public_wss = new WebSocket.Server({ noServer: true });

let column_clients: (null | {alive: boolean, ws: WebSocket})[] = [...new Array(staff.width).map(e => null)];
let rpi_client: (null | {alive: boolean, ws: WebSocket}) = null;

private_wss.on('connection', (ws) => {
  // cache the rpi client (assume any connection on private endpoint is the rpi)
  rpi_client = {alive: true, ws};

  ws.on('message', (str: string) => {
    const msg = strAsMessage(str);

    if(msg.id.name !== 'column'){
      debug.warn('msg rejected. sender id is not column');
      return;
    }
    
    const col = (msg.id.extra) ? Number(msg.id.extra) : undefined;
    if(typeof col === 'undefined'){
      debug.warn(`msg rejected. sender column number is invalid. got: ${col}`);
      return;
    }

    // cache the websocket for the given column
    column_clients[col] = {alive: true, ws};
    rpi_client = {alive: true, ws};

    let response: Message = {
      id: {
        name: 'server',
      },
      timestamp: now(),
    }

    // handle updates
    if(msg.update){
      try {
        if(msg.update.staff){
          throw new TypeError('column controllers cannot send whole staff updates');
        }
        if(msg.update.columns){
          msg.update.columns.forEach(c => {
            if(c.column !== col){
              throw new TypeError('a column controller cannot send updates for other columns');
            }
          });
        }
        if(msg.update.entries){
          msg.update.entries.forEach(e => {
            if(e.column !== col){
              throw new TypeError('a column controller cannot send updates for other columns');
            }
          });
        }      
  
        staff.handleUpdate(msg);  // <-- update the staff data by this message
        updatePublicStaff(staff); // <-- send updated staff over public interface
      } catch (err) {
        debug.error(err);
      }
      
    }

    // handle requests
    if(msg.request){
      response.update = {};
      if(msg.request.staff){
        debug.error('column controllers cannot request whole staff updates');
      }
      if(msg.request.columns){
        response.update.columns = [];
        msg.request.columns.forEach(c => {
          if(!response.update){ throw new TypeError('this should not happen - update'); }
          if(!response.update.columns){ throw new TypeError('this should not happen - columns'); }
          if(c.column !== col){ throw new TypeError('a column controller cannot request updates for other columns'); }
          response.update.columns.push({column: c.column, entries: staff.column(c.column)});
        });
      }
      if(msg.request.entries){
        response.update.entries = [];
        msg.request.entries.forEach(e => {
          if(!response.update){ throw new TypeError('this should not happen - update'); }
          if(!response.update.entries){ throw new TypeError('this should not happen - entries'); }
          if(e.column !== col){ throw new TypeError('a column controller cannot request updates for other columns'); }
          response.update.entries.push({column: e.column, row: e.row, entry: staff.at(e.column, e.row)});
        });
      }
      ws.send(messageToString(response));
    }
  }); 
  ws.on('close', function close() {
    console.log('disconnected');
  });
});

public_wss.on('connection', (ws) => {
  ws.on('message', (str: string) => {
    const msg = strAsMessage(str);

    if(msg.id.name !== 'client'){
      debug.warn('msg rejected. only clients should use the public interface');
      return;
    }

    let response: Message = {
      id: {
        name: 'server',
      },
      timestamp: now(),
    }

    // handle requests
    if(msg.request){
      response.update = {};
      if(msg.request.staff){
        response.update.staff = staff.data();
      }
      if(msg.request.columns){
        response.update.columns = [];
        msg.request.columns.forEach(c => {
          if(!response.update){ throw new TypeError('this should not happen - update'); }
          if(!response.update.columns){ throw new TypeError('this should not happen - columns'); }
          response.update.columns.push({column: c.column, entries: staff.column(c.column)});
        });
      }
      if(msg.request.entries){
        response.update.entries = [];
        msg.request.entries.forEach(e => {
          if(!response.update){ throw new TypeError('this should not happen - update'); }
          if(!response.update.entries){ throw new TypeError('this should not happen - entries'); }
          response.update.entries.push({column: e.column, row: e.row, entry: staff.at(e.column, e.row)});
        });
      }
      ws.send(messageToString(response));
    }

    // handle updates (authorized clients can change the colors on the display)
    if(msg.update){
      if(msg.auth_key){
        if(msg.auth_key === truth){

          // handle full staff updates
          if(msg.update.staff){
            debug.error('full staff updates are not handled for client --> sensor', new Date().toString())
          }

          // handle column updates
          if(msg.update.columns){

            // copy the color data into the model
            msg.update.columns.forEach((col, x) => {
              col.entries.forEach((row, y) => {
                staff.setEntry({row: y, column: x, entry: {color: row.color, note: undefined}});
              });
            });

            // change id of message
            let fwd: Message = {
              ...msg,
              id: {
                name: 'server'
              }
            }

            // fwd to the rpi (who will then fwd to the sensors)
            if(rpi_client){
              if(rpi_client.ws){
                rpi_client.ws.send(messageToString(fwd))
              }
            }
          }

          // handle entry updates
          if(msg.update.entries){
            debug.error('entry updates are not handled for client --> sensor', new Date().toString())
          }

        }else{
          debug.error('unauthorized - incorrect auth key');
        }
      }else{
        debug.error('unauthorized - no auth_key field');
      }
    }

  });
});

// handle authorization
export const upgrade = (request: any, socket: any, head: any) => {
  const pathname = request.url;

  debug.verbose('upgrade', request.headers);

  switch(pathname){
  case '/rainbow-forest/api/v1/private':
    const authorize = () => {
      return new Promise<void>((resolve, reject) => {
        const token = request.headers['rf-auth'];
        if(token === truth){
          resolve();
        }else{
          reject(`incorrect value for rf-auth header. got: ${token}`);
        }
      });
    }
    authorize().then(() => {
      private_wss.handleUpgrade(request, socket, head, (ws) => {
        private_wss.emit('connection', ws, request);
      });
    }).catch(e => {
      socket.destroy();
      console.warn('failed to authorize connection to private api');
    });
    break;

  case '/rainbow-forest/api/v1/public':
    public_wss.handleUpgrade(request, socket, head, (ws) => {
      public_wss.emit('connection', ws, request);
    });
    break;
  
  default:
    socket.destroy();
    break;
  }
};

const now = () => {
  return `todo: timestamp`;
}

// send changes to public clients
export const updatePublic = (msg: Message) => {
  public_wss.clients.forEach((ws) => {
    ws.send(messageToString(msg));
  });
}

export const updatePublicStaff = (staff: Staff) => {
  const msg: Message = {
    id: {
      name: 'server',
    },
    timestamp: now(),
    update: {
      staff: staff.data(),
    }
  };
  updatePublic(msg);
}
