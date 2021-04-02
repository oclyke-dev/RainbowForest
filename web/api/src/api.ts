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
} from '../../common/message';

const private_wss = new WebSocket.Server({ noServer: true });
const public_wss = new WebSocket.Server({ noServer: true });

let column_clients: (null | {alive: boolean, ws: WebSocket})[] = [...new Array(staff.width).map(e => null)];

private_wss.on('connection', (ws) => {
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
