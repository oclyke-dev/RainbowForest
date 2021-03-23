/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "Arduino.h"

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

void updateSensors( void* arg ){

  // initialize column i2c
  COLUMN_WIRE_PORT.begin();

  // initialize sensor string
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(sensors.getControl(), sensors.getNumControlElements()); // have to add leds in main sketch before sensors.begin()
  SensorStatus_e retval = sensors.begin();
  DEBUG_PORT.print("sensors.begin() returned: "); DEBUG_PORT.println(retval);

  while(1){

    // read all sensors in random sequence to make pretty blinking
#ifdef DEBUG_GEN_FAKE_DATA
    sensors.forEachRandOrder(detect_fake, NULL);
#else
    sensors.forEachRandOrder(detect, NULL);
#endif // DEBUG_GEN_FAKE_DATA

    // send updates
    sendColumn();

    // print debug info
    DEBUG_PRINTF(("Column: [ "));
    sensors.forEach(printInfo, NULL);
    DEBUG_PRINTF(("]\n"));
  }
}
