/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// BOARD: ESP32 Dev Module

#include <Wire.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WebSocketsClient.h>   // WebSockets by Markus Sattler (2.x.x for ESP)

#include "src/components/cmt/cmt.h"
#include "src/components/staff/staff.h"
#include "src/components/sensor/sensor.h"
#include "src/components/configuration/configuration.h"

#include "src/components/configuration/network_config.h" // NETWORK_SSID, NETWORK_PASSWORD, PRIVATE_AUTH_HEADER

// #define PRODUCTION // comment out to use development settings

#define CONTROLLER_COLUMN (0)   // indicates which column this controller reads (default if preferences unused)

#define DEV_HOST "10.0.0.29"   // <-- for development set this to the ip address of the machine running the api server
#define DEV_PORT 81

#define DEBUG_OUTPUT // uncomment to enable debug output
#define DEBUG_GEN_FAKE_DATA // uncomment to generate fake column data

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

#define LED0 25
#define LED1 26
#define LED2 27
#define LED3 32

#define STATUS_LED 13

#define BUTTON0 0

#define DATA_PIN 18
#define COLUMN_LEN (STAFF_ROWS)
#define COLUMN_WIRE_PORT Wire
SensorString sensors(COLUMN_LEN, DATA_PIN, &COLUMN_WIRE_PORT);

Staff <staff_data_t> staff;
Staff <bool> detection_ok_staff;

WebSocketsClient private_ws;
volatile bool private_ws_active = false;

Preferences preferences;
#define PREFS_NAMESPACE "rf-col"
#define PREFS_KEY_COL_NUM "column"
uint8_t column_number = CONTROLLER_COLUMN;

volatile bool button0_flag = false;

#ifdef DEBUG_OUTPUT
#define DEBUG_PRINTF(args) DEBUG_PORT.printf args
#else
#define DEBUG_PRINTF(args)
#endif // DEBUG_OUTPUT

void setup() {
#ifdef DEBUG_OUTPUT
  DEBUG_PORT.begin(DEBUG_BAUD);
#endif // DEBUG_OUTPUT

  init_network();
  init_gpio();
  init_staff();
  init_preferences();

  button0_flag = false; // don't allow button presses until setup has completed

  // // begin tasks
  // xTaskCreate(handleUserInput, "handleUserInput", 10000, NULL, 1, NULL);  // handle ISRs
  xTaskCreate(updateSensors, "updateSensors", 10000, NULL, 1, NULL);      // update sensors
}

void loop() {
  private_ws.loop();  // feed the websocket
}