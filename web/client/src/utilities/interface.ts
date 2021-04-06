/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// server address
const host = 'oclyke.dev';
const port = 443;
const protocol = 'wss';

// const host = '10.0.0.29';
// const port = 81;
// const protocol = 'ws';

// root of api
const api = '/rainbow-forest/api/v1';

// endpoints
const public_endpoint = `/public`;   // public namespace

// construct URL for connection
const makeWebSocketURL = (endpoint: string) => {
  return `${protocol}://${host}:${port}${api}${endpoint}`
}

// our interface to the server
export const socket = new WebSocket(makeWebSocketURL(public_endpoint));

socket.onopen = () => {
  console.log('socket opened');
  
  const msg = {
    id: {
      name: 'client'
    },
    timestamp: now(),
    request: {
      staff: []
    }
  }

  socket.send(JSON.stringify(msg))
}
socket.onerror = (e) => {
  console.error(e);
};

const now = () => {
  return 'todo timestamp - client';
}
