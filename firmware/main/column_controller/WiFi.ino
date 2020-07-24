void handleClientDisconnect(void* args, AsyncClient* client){
  DEBUG_PORT.print("Client 0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.println(" disconnected!");
  while(!client->freeable()){};
  client->free();
}

void handleClientAck(void* args, AsyncClient* client, size_t len, uint32_t time){
  pending = false;
}

void handleClientError(void* args, AsyncClient* client, int8_t error){
  DEBUG_PORT.print("Client 0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.print(" error! error = ");
  DEBUG_PORT.print(error);
  DEBUG_PORT.println();

  pending = false;
}

void setColorFromCat(SensorNode* node, size_t idx, void* args){
  uint8_t r = ((cat.rH << 4) | (cat.rL & 0x0F));
  uint8_t g = ((cat.gH << 4) | (cat.gL & 0x0F));
  uint8_t b = ((cat.bH << 4) | (cat.bL & 0x0F));
  
  *(node) = CRGB(g, r, b);
}

void handleCat( void ){
  if(cat.row == COMMAND_REQ_FULL_UPDATE){
    full_update = true;
    return;
  }
  
  if(cat.row == COMMAND_SET_COLUMN_COLOR){
    sensors.forEach(setColorFromCat, NULL);
    return;
  }
  
  uint8_t r = ((cat.rH << 4) | (cat.rL & 0x0F));
  uint8_t g = ((cat.gH << 4) | (cat.gL & 0x0F));
  uint8_t b = ((cat.bH << 4) | (cat.bL & 0x0F));
  
  *(sensors[cat.row]) = CRGB(g, r, b);
}

void handleClientData(void* args, AsyncClient* client, void *data, size_t len){
  const size_t cat_len = (sizeof(cat_t)/sizeof(uint8_t));
  size_t handled = 0;
  while(len >= cat_len){
    len -= cat_len;
    handled += cat_len;
    memcpy((void*)&cat, (void*)(((uint8_t*)data + handled)), cat_len);
    handleCat();
  }
}

void handleClientTimeout(void* args, AsyncClient* client, uint32_t time){
  DEBUG_PORT.print("Client 0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.print(" timeout! time = ");
  DEBUG_PORT.print(time);

  pending = false;
  DEBUG_PORT.print(", pending = ");
  DEBUG_PORT.print((pending) ? "true" : "false");

  DEBUG_PORT.println();
}

void handleClientConnected(void* args, AsyncClient* client){
  pending = false;
}

void setupClient( void ){
  DEBUG_PORT.print("Column Controller Base Client = 0x");
  DEBUG_PORT.print((uint32_t)&client, HEX);
  DEBUG_PORT.println();
  client.onConnect(handleClientConnected, NULL);      //connected
  client.onDisconnect(handleClientDisconnect, NULL);  //disconnected
  client.onAck(handleClientAck, NULL);                //ack received
  client.onError(handleClientError, NULL);            //unsuccessful connect or error
  client.onData(handleClientData, NULL);              //data received (called if onPacket is not used)
  client.onTimeout(handleClientTimeout, NULL);        //ack timeout
}

void connectToServer( void ){
  WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);
  
  while(!wifi_connected){
    static uint32_t connect_wifi = 0;
    if(millis() >= connect_wifi){
      DEBUG_PORT.print("Waiting to connect to SSID: ");
      DEBUG_PORT.println(NETWORK_SSID);
      connect_wifi = millis() + 2000;
    }
  }

  while(!client_connected){
    while(!udp.parsePacket()){
      static uint32_t debug_print_packet = 0;
      if(millis() >= debug_print_packet){
        DEBUG_PORT.println("Waiting to for UDP packet.");
        debug_print_packet = millis() + 500;
      }
    }
    
    ServerIP = udp.remoteIP();
    uint16_t ServerPort = udp.remotePort();
    uint32_t len = udp.available();

    char payload[len+1];
    memset(payload, 0x00, len+1);
    udp.read(payload, len);
    udp.flush();

    DEBUG_PORT.print("UDP packet rx. len = ");
    DEBUG_PORT.print(len);
    DEBUG_PORT.print(", remoteIP = ");
    DEBUG_PORT.print(ServerIP);
    DEBUG_PORT.print(", remotePort = ");
    DEBUG_PORT.print(ServerPort);
    DEBUG_PORT.print(", payload = '");
    DEBUG_PORT.print(String(payload));
    DEBUG_PORT.print("'");
    DEBUG_PORT.println();

    if((ServerPort == NETWORK_UDP_PORT) && (String(payload) == String(NETWORK_PASSWORD))){
      client_connected = client.connect(ServerIP, NETWORK_TCP_PORT);
    }
  }
  udp.stop();
}

void WiFiEvent(WiFiEvent_t event){
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      IP = WiFi.localIP();
      DEBUG_PORT.print("WiFi connected! IP address: ");
      DEBUG_PORT.println(IP);  
      udp.begin(IP, NETWORK_UDP_PORT);
      wifi_connected = true;
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      DEBUG_PORT.println("WiFi lost connection");
      client.close(true);
      client.stop();
      ESP.restart();  // restart to reconnect
      break;
    default: 
      DEBUG_PORT.print("Unhandled WiFi Event: ");
      DEBUG_PORT.println(event);
      break;
  }
}
