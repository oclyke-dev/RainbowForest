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

#include "named_mesh.h" // https://github.com/gmag11/painlessMesh

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

Scheduler meshScheduler;
namedMesh mesh;

DynamicJsonDocument doc(1024);

volatile bool button0 = false;
volatile bool full_update = true;

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

  String column_name = COLUMN_CONTROLLER_PREFIX + String(CONTROLLER_COLUMN);
  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);
  mesh.init(NETWORK_SSID, NETWORK_PASSWORD, &meshScheduler, NETWORK_MESH_PORT);
  mesh.setName(column_name);

  mesh.onReceive([](String &from, String &msg) {
    DEBUG_PORT.printf("Received message by name from: %s, %s\n", from.c_str(), msg.c_str());
    
    deserializeJson(doc, msg.c_str());

    cat.row = doc["row"];
    cat.col = doc["col"];
    cat.rL = doc["rL"];
    cat.rH = doc["rH"];
    cat.gL = doc["gL"];
    cat.gH = doc["gH"];
    cat.bL = doc["bL"];
    cat.bH = doc["bH"];
    
    handleCat();
  });

  mesh.onChangedConnections([]() {
    DEBUG_PORT.printf("Changed connection\n");
    full_update = true;
  });

  xTaskCreate(  updateSensors, "updateSensors", 10000, NULL, 1, NULL);
}

void loop() {
  mesh.update();
  if(button0){
    button0 = false;
  }
}
