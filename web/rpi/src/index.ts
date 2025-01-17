/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import * as express from 'express';
import * as http from 'http';
import * as WebSocket from 'ws';

import staff from '../../common/staff';
import { debug } from '../../common/utility';
import { truth, api_headers } from '../../common/secrets';
import Message, {
  messageToString,
  strAsMessage
} from '../../common/message';

const { spawn } = require('child_process');

// catch any error and make sure to exit with an error code (to trigger system restart)
try {

  // server settings
  const PORT = 81;

  // set up express and http server
  const app = express();
  const server = http.createServer(app);

  // configure websocket for messages to api server (outgoing)
  const API_PROTOCOL = 'wss';
  const API_SERVER = 'oclyke.dev';
  const API_PORT = 443;

  // // you can swap these in as dev values
  // const API_PROTOCOL = 'ws';
  // const API_SERVER = '10.0.0.29';
  // const API_PORT = 81;

  const API_ENDPOINT = '/rainbow-forest/api/v1/private';
  const OUTGOING_WS_URL = `${API_PROTOCOL}://${API_SERVER}:${API_PORT}${API_ENDPOINT}`
  const outgoing_wss = new WebSocket(OUTGOING_WS_URL, {headers: {...api_headers}});

  // configure websocket for messages from sensors (incoming)
  let column_clients: (null | {alive: boolean, ws: WebSocket})[] = [...new Array(staff.width).map(e => null)];

  const incoming_wss = new WebSocket.Server({ noServer: true });
  incoming_wss.on('connection', (ws) => {
    ws.on('message', (str: string) => {
      
      const msg: Message = strAsMessage(str);
      const col_number = Number(msg.id.extra);
      // debug.verbose(`received msg from column ${col_number}:`, msg);
      
      if(!isNaN(col_number) && (col_number < column_clients.length)){
        const cache = {alive: true, ws};
        column_clients[col_number] = cache;
        // debug.verbose(`caching column client ${col_number} (${cache.ws})`);
      }
      outgoing_wss.send(str);
    }); 
    ws.on('close', function close() {
      debug.log('disconnected');
    });
  });

  // configure outgoing ws event handlers
  outgoing_wss.on('error', (e) => {
    debug.error('outgoing ws error! ', e);
  });

  outgoing_wss.on('open', () => {
    debug.log(`opened connection to api sierver (${OUTGOING_WS_URL})`);
  });

  outgoing_wss.on('message', (data) => {
    // cast to Message type
    const msg: Message = strAsMessage(data.toString());

    // handle updates (push them down to column controllers)
    if(msg.update){

      // relay updates from the server to applicable columns
      if(msg.update.columns){

        msg.update.columns.forEach((c, idx) => {
          const client = column_clients[c.column];

          if(client){
            if(client.ws){
              // reduce the message size going to each sensor (only include the relevant column)
              let fwd: Message = {
                id: {
                  name: 'rpi', // impersonating the server -- maybe should
                },
                timestamp: 'todo: timestamp',
                update: {
                  columns: [
                    {column: c.column, entries: c.entries}
                  ]
                }
              }

              client.ws.send(messageToString(fwd));
            }
          }
        })
      }

      // relay entry updates from the server to applicable columns
      if(msg.update.entries){
        const columns = [...new Set(msg.update.entries.map(e => e.column))];  // get unique columns
        debug.verbose('sending entry updates to columns: ', columns);
        columns.forEach(n => {                                                // once per column relay this update
          const client = column_clients[n];
          if(client){
            if(client.ws){
              client.ws.send(data);
            }
          }
        })
      }

      // relay whole staff updates
      if(msg.update.staff){
        debug.verbose('sending full staff update to columns');
        column_clients.forEach(client => {
          if(client){
            if(client.ws){
              client.ws.send(data);
            }
          }
        })
      }
    }

    // for now request messages are unhandled
    if(msg.request){
      debug.error('unhandled request by api server');
    }

  });


  // handle upgrade requests from sensors on the private endpoint
  server.on('upgrade', (request: any, socket: any, head: any) => {
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
        incoming_wss.handleUpgrade(request, socket, head, (ws) => {
          incoming_wss.emit('connection', ws, request);
        });
      }).catch(e => {
        socket.destroy();
        console.warn('failed to authorize connection to private api');
      });
      break;
    
    default:
      socket.destroy();
      break;
    }
  });

  // redirect visitors to the client app
  app.get('/*', (req, res) => {
    res.redirect('https://oclyke.dev/rainbow-forest')
  });

  process.on('SIGTERM', () => {
    process.exit(2);
  })

  server.listen(PORT, () => {

    // start the server
    const address = server.address();
    let port = undefined;
    if(address){
      if(typeof(address) !== 'string'){
        port = address.port;
      }
    }
    console.log(`listening on *:${port}`);
  });
} catch {
  process.exit(1);
}


var keypress = require('keypress');
// use decoration to enable stdin to start sending ya events 
keypress(process.stdin);
// listen for the "keypress" event
process.stdin.on('keypress', function (ch, key) {
    console.log('got "keypress"', key, ch);
    if (key && key.ctrl && key.name == 'c') {
      process.stdin.pause();
      process.exit(0);
    }

    if(ch){
      const n = Number(ch);
      if(n >= 0 && n < 7){
        playColumn([...new Array(7)].map((e, idx) => { return (n >= idx) ? 0 : null}));
      }
    }
});

process.stdin.setRawMode(true);
process.stdin.resume();

type Instrument = {
  notes: {
    path: string,
    // buffer: AudioBuffer | undefined,
  }[],
}

const milibels = 0;

let banjo: Instrument = {
  notes: [
    {path: '/home/pi/RainbowForest/web/rpi/src/assets/audio/banjo/banjo_C3_very-long_piano_normal.mp3'},
    {path: '/home/pi/RainbowForest/web/rpi/src/assets/audio/banjo/banjo_D3_very-long_piano_normal.mp3'},
    {path: '/home/pi/RainbowForest/web/rpi/src/assets/audio/banjo/banjo_E3_very-long_piano_normal.mp3'},
    {path: '/home/pi/RainbowForest/web/rpi/src/assets/audio/banjo/banjo_F3_very-long_piano_normal.mp3'},
    {path: '/home/pi/RainbowForest/web/rpi/src/assets/audio/banjo/banjo_G3_very-long_piano_normal.mp3'},
    {path: '/home/pi/RainbowForest/web/rpi/src/assets/audio/banjo/banjo_A3_very-long_piano_normal.mp3'},
    {path: '/home/pi/RainbowForest/web/rpi/src/assets/audio/banjo/banjo_B3_very-long_piano_normal.mp3'},
    {path: '/home/pi/RainbowForest/web/rpi/src/assets/audio/banjo/banjo_C3_very-long_piano_normal.mp3'},
  ]
}

const instruments: Instrument[] = [
  banjo,
]

const logAsciiBuffer = (b: any) => {
  console.log(String.fromCharCode(...b));
}

// play a column
const playColumn = (col: (number | null)[]) => {

  col.forEach((i, idx) => {
    if(i !== null){
      const path = instruments[i].notes[idx].path;
      let player = spawn('omxplayer', ['--no-keys', '--vol', `${milibels}`, path, '&']);
      player.stdout.on('data', (data: any) => {
        logAsciiBuffer(data);
      })
      player.stderr.on('data', (data: any) => {
        console.warn('error: ', logAsciiBuffer(data))
      })
      player.on('close', (code: any) => {
        console.log(`child process exited with code ${code}`);
      });
    }
  })

  console.log('playing: ', col);

}




