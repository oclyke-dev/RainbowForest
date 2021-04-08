/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import * as express from 'express';
import * as http from 'http';

import {
  upgrade as transportUpgrade,
} from './api';

// server settings
const PORT = 81;

// set up express and http server
const app = express();
const server = http.createServer(app);

// handle upgrade requests to authorize private endpoint
server.on('upgrade', (request, socket, head) => {
  transportUpgrade(request, socket, head);
});

// redirect visitors to the client app
app.get('/*', (req, res) => {
  res.redirect('https://oclyke.dev/rainbow-forest')
});

// start the server
server.listen(PORT, () => {
  const address = server.address();
  let port = undefined;
  if(address){
    if(typeof(address) !== 'string'){
      port = address.port;
    }
  }
  console.log(`listening on *:${port}`);
});
