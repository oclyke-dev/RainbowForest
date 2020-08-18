/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// BOARD: Adafruit ESP32 Feather

#include "src/components/uart_bridge/uart_bridge.h"
#include "src/components/cart/cart.h"
#include "src/components/cat/cat.h"

#include "src/components/configuration/configuration.h"

#include "WiFi.h"
#include "WiFiUdp.h"
#include "AsyncTCP.h" // https://github.com/me-no-dev/AsyncTCP

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

#define BRIDGE_PORT Serial1
#define BRIDGE_BAUD (115200)
UARTBridge <cart_t> cartBridge(BRIDGE_PORT);
UARTBridge <cat_t> catBridge(BRIDGE_PORT);
cart_t cart;
cat_t cat;

IPAddress IP;
WiFiUDP udp;
AsyncServer server(NETWORK_TCP_PORT);

AsyncClient* column_clients[STAFF_COLS];

volatile bool button0 = false;
void IRAM_ATTR button0ISR() {
  button0 = true;
}

void onCatReception(cat_t* cat, void* args){
  if(cat->col < STAFF_COLS){
    AsyncClient* client = column_clients[cat->col];
    if(client){
      DEBUG_PORT.print("conductor --> column[");
      DEBUG_PORT.print(cat->col);
      DEBUG_PORT.print("] (client 0x");
      printHex8(&DEBUG_PORT, (uint32_t)client);
      DEBUG_PORT.print("), {");
      DEBUG_PORT.print(" col: ");
      DEBUG_PORT.print(cat->col);
      DEBUG_PORT.print(", row: ");
      DEBUG_PORT.print(cat->row);
      DEBUG_PORT.print(", r: ");
      DEBUG_PORT.print(((cat->rH) << 4) | ((cat->rL) & 0x0F));
      DEBUG_PORT.print(", g: ");
      DEBUG_PORT.print(((cat->gH) << 4) | ((cat->gL) & 0x0F));
      DEBUG_PORT.print(", b: ");
      DEBUG_PORT.print(((cat->bH) << 4) | ((cat->bL) & 0x0F));
      DEBUG_PORT.print(" }");
      DEBUG_PORT.println();
      
      client->write((const char*)cat, (sizeof(cat_t)/sizeof(uint8_t)));
      return;
    }
  }

  DEBUG_PORT.println("received an invalid cat");
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
  BRIDGE_PORT.begin(BRIDGE_BAUD);

  pinMode(0, INPUT_PULLUP);
  attachInterrupt(0, button0ISR, RISING);

  catBridge.onReceive(onCatReception, NULL);

  WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);

  DEBUG_PORT.print("Connecting to WiFi: ");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        DEBUG_PORT.print(".");
    }

    DEBUG_PORT.println("");
    DEBUG_PORT.println("WiFi connected");
    DEBUG_PORT.println("IP address: ");
    DEBUG_PORT.println(WiFi.localIP());
  
  IP = WiFi.localIP();

  server.onClient(handleClientConnected, NULL);
  server.begin();
  
  udp.begin(IP, NETWORK_UDP_PORT);
  xTaskCreate(broadcastIP, "broadcastIP", 10000, NULL, 1, NULL);
}

void loop() {
  catBridge.check();
  if(button0){
    DEBUG_PORT.println("button0 released");
    button0 = false;
  }

//  if(Serial.available()){
//    char command = Serial.read();
//    if(command == 'r'){
//      cat.col = 15;
//      cat.row = COMMAND_REQ_FULL_UPDATE;
//      cat.rH = 0;
//      cat.rL = 0;
//      cat.gH = 0;
//      cat.gL = 0;
//      cat.bH = 0;
//      cat.bL = 0;
//      onCatReception(&cat, NULL);
//    }else if(command == 's'){
//      cat.col = 15;
//      cat.row = COMMAND_SET_COLUMN_COLOR;
//      cat.rH = 0x0F;
//      cat.rL = 0x0F;
//      cat.gH = 0;
//      cat.gL = 0;
//      cat.bH = 0;
//      cat.bL = 0;
//      onCatReception(&cat, NULL);
//    }else if(command == 'i'){
//      cat.col = 15;
//      cat.row = 3;
//      cat.rH = 0;
//      cat.rL = 0;
//      cat.gH = 0x08;
//      cat.gL = 0x0F;
//      cat.bH = 0x0A;
//      cat.bL = 0x0F;
//      onCatReception(&cat, NULL);
//    }
//    
//  }
}
