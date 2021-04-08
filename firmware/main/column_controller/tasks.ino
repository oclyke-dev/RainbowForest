/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "Arduino.h"

#define I2C_SDA (23)
#define I2C_SCL (22)

void handleUserInput( void * arg ) {

  while(1){
    vTaskDelay(50 / portTICK_PERIOD_MS);

    // button 0 changes the configured column number
    if(button0_flag){
      button0_flag = false;
      static uint32_t debounce = 0;
      if(millis() > debounce){
        debounce = millis() + 50;
        column_number++;
        if(column_number >= STAFF_COLS){
          column_number = 0;
        }
        setColumnNumber(column_number);
      }
    }
  }
}

void detectAndSend(SensorNode* node, size_t idx, void* args){
#ifdef DEBUG_GEN_FAKE_DATA
  detect_fake(node, idx, args);
#else
  detect(node, idx, args);
#endif // DEBUG_GEN_FAKE_DATA
  sendRow(idx); // send updates piecewise
}

void updateSensors( void* arg ){

  // initialize column i2c
  COLUMN_WIRE_PORT.begin(I2C_SDA, I2C_SCL);

  // initialize sensor string
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(sensors.getControl(), sensors.getNumControlElements()); // have to add leds in main sketch before sensors.begin()
  SensorStatus_e retval = sensors.begin();
  DEBUG_PORT.print("sensors.begin() returned: "); DEBUG_PORT.println(retval);

  while(1){

    // read all sensors in random sequence to make pretty blinking
    sensors.forEachRandOrder(detectAndSend, NULL);

    // print debug info
    DEBUG_PRINTF(("Column: [ "));
    sensors.forEach(printInfo, NULL);
    DEBUG_PRINTF(("]\n"));
  }
}
