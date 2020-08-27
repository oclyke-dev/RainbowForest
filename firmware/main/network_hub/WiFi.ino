/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

void handleClientDisconnect(void* args, AsyncClient* client){
  DEBUG_PORT.print("Client 0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.println(" disconnected!");
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
  memcpy((void*)&cart, data, len);
  if(cart.col < STAFF_COLS){
    DEBUG_PORT.print("column[");
    DEBUG_PORT.print(cart.col);
    DEBUG_PORT.print("] (client 0x");
    printHex8(&DEBUG_PORT, (uint32_t)client);
    DEBUG_PORT.print(") --> conductor , {");
    DEBUG_PORT.print(" col: ");
    DEBUG_PORT.print(cart.col);
    DEBUG_PORT.print(", row: ");
    DEBUG_PORT.print(cart.row);
    DEBUG_PORT.print(", val: ");
    DEBUG_PORT.print(cart.val);
    DEBUG_PORT.print(" }");
    DEBUG_PORT.println();
    
    column_clients[cart.col] = client;
    cartBridge.send(&cart);
  }
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

void WiFiEvent(WiFiEvent_t event){
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      IP = WiFi.localIP();
      DEBUG_PORT.print("WiFi connected! IP address: ");
      DEBUG_PORT.println(IP);  
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      DEBUG_PORT.println("WiFi lost connection");
      ESP.restart();  // restart to reconnect
      break;
    default: 
      DEBUG_PORT.print("Unhandled WiFi Event: ");
      DEBUG_PORT.println(event);
      break;
  }
}
