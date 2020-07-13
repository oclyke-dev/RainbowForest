/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// BOARD: Adafruit ESP32 Feather

#include "src/components/cart/cart.h"
#include "src/components/sensor/sensor.h"
#include "src/components/configuration/configuration.h"

#include "WiFi.h"
#include "WiFiUdp.h"
#include "AsyncTCP.h" // https://github.com/me-no-dev/AsyncTCP

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

#define CONTROLLER_COLUMN (15) // indicates which column this controller reads

#define DATA_PIN 18
#define COLUMN_LEN (7)
SensorString sensors(COLUMN_LEN, DATA_PIN);

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
  node->read();
  
  rgb_f_t rgb;
  rgb.r = rgbElemByUI16(node->getRed());
  rgb.g = rgbElemByUI16(node->getGreen());
  rgb.b = rgbElemByUI16(node->getBlue());

  DEBUG_PORT.print(idx);
  DEBUG_PORT.print(": ");
  
  size_t val;
  if(COLOR_DETECT_OK == detectedColor(&rgb, &val)){
    sendRV(idx, val);
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

  while(1){
    DEBUG_PORT.print("Column: [");
    sensors.forEachRandOrder(detectAndTransmit);
    DEBUG_PORT.println("]");
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
      for(size_t idx = 0; idx < COLUMN_LEN; idx++){
        SensorNode* node = sensors[idx];
        *(node) = (cart.col & (0x01 << idx)) ? CRGB(127, 127, 127) : CRGB(0, 0, 0);
      }
    }
  }
}
