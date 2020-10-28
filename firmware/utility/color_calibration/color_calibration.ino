/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "src/components/sensor/sensor.h"
#include "src/components/configuration/configuration.h"

#include <Wire.h>
#include "wiring_private.h" // pinPeripheral() function

#define CALIBRATION_NODE 1 // which sensor in the string to use for calibration

#define DEBUG_PORT SerialUSB
#define DEBUG_BAUD (115200)

#define COLUMN_SDA_PIN 4
#define COLUMN_SCL_PIN 3
TwoWire ColumnWire(&sercom0, COLUMN_SDA_PIN, COLUMN_SCL_PIN);

#define DATA_PIN 6
#define COLUMN_LEN (STAFF_ROWS)
SensorString sensors(COLUMN_LEN, DATA_PIN, &ColumnWire);

const size_t rolling_avg_len = 16;
double mag_history[rolling_avg_len];

void printHex4(Stream* stream, uint32_t val){
  if(val < 0x1000){ stream->print(0); }
  if(val < 0x100){ stream->print(0); }
  if(val < 0x10){ stream->print(0); }
  stream->print(val, HEX);
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
  DEBUG_PORT.println("Color Calibration\n");

  ColumnWire.begin();
  pinPeripheral(COLUMN_SDA_PIN, PIO_SERCOM);
  pinPeripheral(COLUMN_SCL_PIN, PIO_SERCOM);
  
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(sensors.getControl(), sensors.getNumControlElements()); // have to add leds in main sketch before sensors.begin()
  SensorStatus_e retval = sensors.begin();
  DEBUG_PORT.print("sensors.begin() returned: "); DEBUG_PORT.println(retval);

}

void loop() {

  // SensorNode* node = sensors[CALIBRATION_NODE];
  
  static uint8_t node_idx = 0;
  SensorNode* node = sensors[node_idx];
  
  node->read();
  
  rgb_f_t rgb;
  rgb.r = rgbElemByUI16(node->getRed());
  rgb.g = rgbElemByUI16(node->getGreen());
  rgb.b = rgbElemByUI16(node->getBlue());

  hsv_f_t hsv = rgbToHsv(&rgb);
  double mag = rgbToMagnitude(&rgb);

  double rolling_avg_mag = 0.0f;
  for(size_t idx = rolling_avg_len - 1; idx > 0; idx--){
    double val = mag_history[idx-1];
    rolling_avg_mag += val;
    mag_history[idx] = val; 
  }
  rolling_avg_mag /= rolling_avg_len;
  mag_history[0] = mag;

  DEBUG_PORT.print("node_idx: ");
  DEBUG_PORT.print(node_idx);
  DEBUG_PORT.print(", ");

  size_t val;
  if(COLOR_DETECT_OK == detectedColor(&rgb, &val)){
    DEBUG_PORT.print(detectable_colors[val].name);
  }else{
    DEBUG_PORT.print("unknown");
  }

  DEBUG_PORT.print(" {");
  DEBUG_PORT.print(" r: 0x");
  printHex4(&DEBUG_PORT, node->getRed());
  DEBUG_PORT.print(", g: 0x");
  printHex4(&DEBUG_PORT, node->getGreen());
  DEBUG_PORT.print(", b: 0x");
  printHex4(&DEBUG_PORT, node->getBlue());
  DEBUG_PORT.print(" }");

  DEBUG_PORT.print("    ");

  DEBUG_PORT.print("(");
  DEBUG_PORT.print(" h: ");
  DEBUG_PORT.print(hsv.h);
  DEBUG_PORT.print(", mag: ");
  DEBUG_PORT.print(mag, 5);
  DEBUG_PORT.print(" [");
  DEBUG_PORT.print(rolling_avg_mag, 5);
  DEBUG_PORT.print("]");
  DEBUG_PORT.print(" )");
  
  DEBUG_PORT.println();

  node_idx++;
  if(node_idx >= COLUMN_LEN){
    node_idx = 0;
  }
}
