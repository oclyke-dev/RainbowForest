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
#include "Preferences.h"

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

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

#define CONTROLLER_COLUMN (0) // indicates which column this controller reads (default if EEPROM unused)

#define PREF_NAMESPACE "rf_cc"
#define PREF_KEY_CONTROLLER_COL "column"
Preferences preferences;

bool full_update = true;

volatile bool pending = true;
cart_t cart;
cat_t cat;

void setColumnNumber( uint8_t col ){
  if(col >= STAFF_COLS){ return; }
  cart.col = col;
  preferences.putUInt(PREF_KEY_CONTROLLER_COL, cart.col);
  DEBUG_PORT.print("Set column to ");
  DEBUG_PORT.println(cart.col);
}

void IRAM_ATTR button0ISR() {
  button0 = true;
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);

  preferences.begin(PREF_NAMESPACE, false);

  pinMode(0, INPUT_PULLUP);
  attachInterrupt(0, button0ISR, RISING);

  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  staff.setDebugStream(DEBUG_PORT);
  staff.setSize(1, STAFF_ROWS);
  prev_staff.setSize(1, STAFF_ROWS);
  detection_ok_staff.setSize(1, STAFF_ROWS);

  memset((void*)&cart, 0x00, (sizeof(cart_t)/sizeof(uint8_t)));
  cart.col = preferences.getUInt(PREF_KEY_CONTROLLER_COL, CONTROLLER_COLUMN);

  DEBUG_PORT.print("Column Controller (");
  DEBUG_PORT.print(cart.col);
  DEBUG_PORT.println(")");

  FastLED.addLeds<WS2811, DATA_PIN, RGB>(sensors.getControl(), sensors.getNumControlElements()); // have to add leds in main sketch before sensors.begin()
  SensorStatus_e retval = sensors.begin();
  DEBUG_PORT.print("sensors.begin() returned: "); DEBUG_PORT.println(retval);

  WiFi.onEvent(WiFiEvent);
  setupClient();
  connectToServer();

  xTaskCreate(  updateSensors, "updateSensors", 10000, NULL, 1, NULL);

  button0 = false; // don't allow button presses until setup has completed
}

void loop() {
  if(button0){
    button0 = false;
    static uint32_t debounce = 0;
    if(millis() > debounce){
      debounce = millis() + 50;
      cart.col++;
      if(cart.col >= STAFF_COLS){
        cart.col = 0;
      }
      setColumnNumber(cart.col);
      for(size_t idx = 0; idx < COLUMN_LEN; idx++){
        SensorNode* node = sensors[idx];
        *(node) = (cart.col & (0x01 << idx)) ? CRGB(127, 127, 127) : CRGB(0, 0, 0);
      }
    }
    
  }
}
