/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

void handleClientDisconnect(void* args, AsyncClient* client){
  DEBUG_PORT.print("Client 0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.println(" disconnected!");
  while(!client->freeable()){};
  client->free();
}

void handleClientAck(void* args, AsyncClient* client, size_t len, uint32_t time){
  DEBUG_PORT.print("Client 0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.print(" ACKed! len = ");
  DEBUG_PORT.print(len);
  DEBUG_PORT.print(", time = ");
  DEBUG_PORT.print(time);
  DEBUG_PORT.println();
}

void handleClientError(void* args, AsyncClient* client, int8_t error){
  DEBUG_PORT.print("Client 0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.print(" error! error = ");
  DEBUG_PORT.print(error);
  DEBUG_PORT.println();
}

void handleClientData(void* args, AsyncClient* client, void *data, size_t len){
//  DEBUG_PORT.print("Client 0x");
//  DEBUG_PORT.print((uint32_t)client, HEX);
//  DEBUG_PORT.print(" onData!");
//  DEBUG_PORT.println();

  memcpy((void*)&cart, data, len);
  cartBridge.send(&cart);

//  DEBUG_PORT.print("col: "); DEBUG_PORT.println(cart.col);
//  DEBUG_PORT.print("row: "); DEBUG_PORT.println(cart.row);
//  DEBUG_PORT.print("val: "); DEBUG_PORT.println(cart.val);
//
//  DEBUG_PORT.print("Space in TCP window: ");
//  DEBUG_PORT.print(client->space());
//  DEBUG_PORT.println();  
}

void handleClientTimeout(void* args, AsyncClient* client, uint32_t time){
  DEBUG_PORT.print("Client 0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.print(" timeout! time = ");
  DEBUG_PORT.print(time);
  DEBUG_PORT.println();
}

void handleClientConnected(void* args, AsyncClient* client){
  DEBUG_PORT.print("Client 0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.println(" connected!");

  client->onDisconnect(handleClientDisconnect, NULL); //disconnected
  client->onAck(handleClientAck, NULL);               //ack received
  client->onError(handleClientError, NULL);           //unsuccessful connect or error
  client->onData(handleClientData, NULL);             //data received (called if onPacket is not used)
  client->onTimeout(handleClientTimeout, NULL);       //ack timeout
}

void broadcastIP( void * params ){
  while(1){
    udp.beginPacket(NETWORK_UDP_ADDRESS, NETWORK_UDP_PORT);
    udp.print(NETWORK_PASSWORD);
    udp.endPacket();
    delay(NETWORK_UDP_BROADCAST_PERIOD);
  }
  vTaskDelete( NULL );
}
