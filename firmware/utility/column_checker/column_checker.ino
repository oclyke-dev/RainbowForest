#include "src/components/sensor/sensor.h"
#include "src/components/configuration/configuration.h"

#include <Wire.h>
#include "wiring_private.h" // pinPeripheral() function

#define DEBUG_PORT SerialUSB
#define DEBUG_BAUD (115200)

#define COLUMN_SDA_PIN 4
#define COLUMN_SCL_PIN 3
TwoWire ColumnWire(&sercom0, COLUMN_SDA_PIN, COLUMN_SCL_PIN);

#define DATA_PIN 6
#define COLUMN_LEN (7)
SensorString sensors(COLUMN_LEN, DATA_PIN, &ColumnWire);

uint32_t errors = 0x00;

void readAndReport(SensorNode* node, size_t idx, void* args){
  node->read();

  rgb_f_t rgb;
  rgb.r = rgbElemByUI16(node->getRed());
  rgb.g = rgbElemByUI16(node->getGreen());
  rgb.b = rgbElemByUI16(node->getBlue());

  DEBUG_PORT.print("red: ");
  DEBUG_PORT.print(node->getRed());

  DEBUG_PORT.print(idx);
  DEBUG_PORT.print(": ");

  size_t val;
  if(COLOR_DETECT_OK == detectedColor(&rgb, &val)){
    DEBUG_PORT.print(detectable_colors[val].name);
  }else{
    DEBUG_PORT.print("unknown");
    errors |= (0x01 << idx);
  }

  DEBUG_PORT.print(", ");
}

void testString( void ){
  errors = 0x00;
  DEBUG_PORT.print("Column: [");
  uint32_t tStart = millis();
  sensors.forEach(readAndReport);
  uint32_t tStop = millis();
  DEBUG_PORT.println("]");
  uint32_t took = (tStop - tStart);

  bool timed_out = (took > (COLUMN_LEN * (500 + 100)));

  if((!timed_out) && (errors == 0x00)){
    DEBUG_PORT.println("All Good!");
    return;
  }
  
  if(timed_out){
    DEBUG_PORT.print("timeout. ");
  }
  if(errors){
    DEBUG_PORT.print("failed: errors: 0b");
    for(size_t idx = 0; idx < 8; idx++){
      if(errors < (((uint32_t)0x01) << idx)){
        DEBUG_PORT.print(0);
      }
    }
    DEBUG_PORT.println(errors, BIN);
  }
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);

  ColumnWire.begin();
  pinPeripheral(COLUMN_SDA_PIN, PIO_SERCOM);
  pinPeripheral(COLUMN_SCL_PIN, PIO_SERCOM);

  FastLED.addLeds<WS2811, DATA_PIN, RGB>(sensors.getControl(), sensors.getNumControlElements()); // have to add leds in main sketch before sensors.begin()
  SensorStatus_e retval = sensors.begin();
  DEBUG_PORT.print("sensors.begin() returned: "); DEBUG_PORT.println(retval);
  DEBUG_PORT.println();
}

void loop() {
  testString();
}
