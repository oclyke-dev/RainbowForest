/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// BOARD: Adafruit ESP32 Feather

#include "src/components/cat/cat.h"
#include "src/components/cart/cart.h"
#include "src/components/staff/staff.h"
#include "src/components/sensor/sensor.h"
#include "src/components/configuration/configuration.h"

#include "WiFi.h"
#include "WiFiUdp.h"
#include "AsyncTCP.h" // https://github.com/me-no-dev/AsyncTCP

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

#define CONTROLLER_COLUMN (0) // indicates which column this controller reads

#define STATUS_LED 13

#define DATA_PIN 18
#define COLUMN_LEN (STAFF_ROWS)
SensorString sensors(COLUMN_LEN, DATA_PIN);

Staff <staff_data_t> staff;
Staff <staff_data_t> prev_staff;
Staff <bool> detection_ok_staff;

volatile bool button0 = false;
volatile bool wifi_connected = false;
volatile bool client_connected = false;
IPAddress IP;
IPAddress ServerIP;
WiFiUDP udp;
AsyncClient client;

bool full_update = true;

volatile bool pending = true;
cart_t cart;
cat_t cat;

void setColumnNumber( uint8_t col ){
  if(col >= STAFF_COLS){ return; }
  cart.col = col;
  DEBUG_PORT.print("Set column to ");
  DEBUG_PORT.println(cart.col);
}

void IRAM_ATTR button0ISR() {
  button0 = true;
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);

  pinMode(0, INPUT_PULLUP);
  attachInterrupt(0, button0ISR, RISING);

  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  staff.setDebugStream(DEBUG_PORT);
  staff.setSize(1, STAFF_ROWS);
  prev_staff.setSize(1, STAFF_ROWS);
  detection_ok_staff.setSize(1, STAFF_ROWS);

  memset((void*)&cart, 0x00, (sizeof(cart_t)/sizeof(uint8_t)));
  setColumnNumber(CONTROLLER_COLUMN);

  DEBUG_PORT.print("Column Controller (");
  DEBUG_PORT.print(cart.col);
  DEBUG_PORT.println(")");

  // startup delay based on column number
  delay(1000 * CONTROLLER_COLUMN);

  WiFi.onEvent(WiFiEvent);
  setupClient();
  connectToServer();

  xTaskCreate(  updateSensors, "updateSensors", 10000, NULL, 1, NULL);
}

void loop() {
  if(button0){
    button0 = false;
  }
}
