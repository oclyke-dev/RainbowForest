/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// BOARD: Adafruit ESP32 Feather

#include "src/components/uart_bridge/uart_bridge.h"
#include "src/components/cart/cart.h"

#include "src/components/configuration/configuration.h"

#include "WiFi.h"
#include "WiFiUdp.h"
#include "AsyncTCP.h" // https://github.com/me-no-dev/AsyncTCP

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

#define BRIDGE_PORT Serial1
#define BRIDGE_BAUD (115200)
UARTBridge <cart_t> cartBridge(BRIDGE_PORT);
cart_t cart;

IPAddress IP;
WiFiUDP udp;
AsyncServer server(NETWORK_TCP_PORT);

void random_cart( void ){
  cart.col = random(0, STAFF_COLS);
  cart.row = random(0, STAFF_ROWS);
  cart.val = random(0, STAFF_VALS);
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

void handleClientDisconnect(void* args, AsyncClient* client){
  DEBUG_PORT.print("Client 0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.println(" disconnected!");
  // todo: idk
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

void handleClientPacket(void* args, AsyncClient* client, struct pbuf *pb){
  DEBUG_PORT.print("0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.print(" received a packet. pb* = 0x");
  DEBUG_PORT.print((uint32_t)pb, HEX);
  DEBUG_PORT.print(", len = ");
  DEBUG_PORT.print(pb->len);
  DEBUG_PORT.print(", tot_len = ");
  DEBUG_PORT.print(pb->tot_len);
  DEBUG_PORT.println();
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
  client->onPacket(handleClientPacket, NULL);         //data received
  client->onTimeout(handleClientTimeout, NULL);       //ack timeout
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
  BRIDGE_PORT.begin(BRIDGE_BAUD);

  WiFi.softAP(NETWORK_SSID, NETWORK_PASSWORD);
  IP = WiFi.softAPIP();

  server.onClient(handleClientConnected, NULL);
  server.begin();
  
  udp.begin(IP, NETWORK_UDP_PORT);
  xTaskCreate(broadcastIP, "broadcastIP", 10000, NULL, 1, NULL);
}

void loop() {
  random_cart();
  cartBridge.send(&cart);
  delay(200);
}
