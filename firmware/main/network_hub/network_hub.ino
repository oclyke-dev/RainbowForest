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

volatile bool button0 = false;
void IRAM_ATTR button0ISR() {
  button0 = true;
}

void random_cart( void ){
  cart.col = random(0, STAFF_COLS);
  cart.row = random(0, STAFF_ROWS);
  cart.val = random(0, STAFF_VALS);
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
  BRIDGE_PORT.begin(BRIDGE_BAUD);

  pinMode(0, INPUT_PULLUP);
  attachInterrupt(0, button0ISR, RISING);

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

  if(button0){
    DEBUG_PORT.println("button0 released");
    button0 = false;
  }
}
