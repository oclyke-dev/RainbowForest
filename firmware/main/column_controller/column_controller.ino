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

#define CONTROLLER_COLUMN (0) // indicates which column this controller reads
#define PREF_KEY_CONTROLLER_COL "column"

#define PREF_NAMESPACE "rf_cc"

#define DATA_PIN 18
#define COLUMN_LEN (STAFF_ROWS)
SensorString sensors(COLUMN_LEN, DATA_PIN);

Staff <staff_data_t> staff;

volatile bool button0 = false;
volatile bool wifi_connected = false;
volatile bool client_connected = false;
IPAddress IP;
IPAddress ServerIP;
WiFiUDP udp;
AsyncClient client;

volatile bool pending = true;
cart_t cart;
cat_t cat;

Preferences preferences;

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

  staff.setDebugStream(DEBUG_PORT);
  staff.setSize(1, STAFF_ROWS);

  memset((void*)&cart, 0x00, (sizeof(cart_t)/sizeof(uint8_t)));
  cart.col = preferences.getUInt(PREF_KEY_CONTROLLER_COL, CONTROLLER_COLUMN);

  DEBUG_PORT.print("Column Controller (");
  DEBUG_PORT.print(cart.col);
  DEBUG_PORT.println(")");

  WiFi.onEvent(WiFiEvent);
  setupClient();
  connectToServer();

  xTaskCreate(  updateSensors, "updateSensors", 10000, NULL, 1, NULL);
}

void sendRV(uint8_t row, uint8_t val){
  while(pending){};
  cart.row = row;
  cart.val = val;
  client.write((const char*)&cart, (sizeof(cart_t)/sizeof(uint8_t)));
  pending = true;
}

void detectAndTransmit(SensorNode* node, size_t idx, void* args){
  bool first_update = *((bool*)args);
  
  node->read();
  
  rgb_f_t rgb;
  rgb.r = rgbElemByUI16(node->getRed());
  rgb.g = rgbElemByUI16(node->getGreen());
  rgb.b = rgbElemByUI16(node->getBlue());

  DEBUG_PORT.print(idx);
  DEBUG_PORT.print(": ");
  
  size_t val;
  if(COLOR_DETECT_OK == detectedColor(&rgb, &val)){
    if((val != staff[0][idx]) || (first_update)){
      staff[0][idx] = (staff_data_t)val;
      sendRV(idx, val);
      DEBUG_PORT.print("(X) ");
      if(first_update){
        DEBUG_PORT.print("(F) ");
      }
    }
    DEBUG_PORT.print(detectable_colors[val].name);
  }else{
    DEBUG_PORT.print("unknown");
  }

  DEBUG_PORT.print(", ");

  delay(esp_random() & (uint32_t)0x0000000F);
}

void updateSensors( void* args ){

  FastLED.addLeds<WS2811, DATA_PIN, RGB>(sensors.getControl(), sensors.getNumControlElements()); // have to add leds in main sketch before sensors.begin()
  SensorStatus_e retval = sensors.begin();
  DEBUG_PORT.print("sensors.begin() returned: "); DEBUG_PORT.println(retval);

  bool first_update = true;
  while(1){
    DEBUG_PORT.print("Column: [");
    sensors.forEachRandOrder(detectAndTransmit, &first_update);
    DEBUG_PORT.println("]");
    if(first_update){
      first_update = false;
    }
  }
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
