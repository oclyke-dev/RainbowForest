/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// BOARD: Adafruit ESP32 Feather

#include "src/components/cart/cart.h"

#include "src/components/configuration/configuration.h"

#include "WiFi.h"
#include "WiFiUdp.h"
#include "AsyncTCP.h" // https://github.com/me-no-dev/AsyncTCP

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

#define CONTROLLER_COLUMN (0) // indicates which column this controller reads

volatile bool button0 = false;
volatile bool wifi_connected = false;
volatile bool client_connected = false;
IPAddress IP;
IPAddress ServerIP;
WiFiUDP udp;
AsyncClient client;

volatile bool pending = true;
cart_t cart;

void IRAM_ATTR button0ISR() {
  button0 = true;
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);

  pinMode(0, INPUT_PULLUP);
  attachInterrupt(0, button0ISR, RISING);

  memset((void*)&cart, 0x00, (sizeof(cart_t)/sizeof(uint8_t)));
  cart.col = CONTROLLER_COLUMN;

  WiFi.onEvent(WiFiEvent);
  setupClient();
  connectToServer();
}

void sendRV(uint8_t row, uint8_t val){
  while(pending){};
  cart.row = row;
  cart.val = val;
  client.write((const char*)&cart, (sizeof(cart_t)/sizeof(uint8_t)));
  pending = true;
}

void loop() {
  static uint32_t send_message = 0;
  if(button0 || (millis() >= send_message)){
    static uint8_t row = 0;
    sendRV(row++, 3);
    if(row >= STAFF_ROWS){
      row = 0;
    }
    button0 = false;
    send_message = millis() + 300;
  }
}
