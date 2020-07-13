#include "src/components/configuration/configuration.h"
#include "src/components/sensor/sensor.h"

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

#define DATA_PIN 18
#define COLUMN_LEN (7)
SensorString sensors(COLUMN_LEN, DATA_PIN);

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);

  FastLED.addLeds<WS2811, DATA_PIN, RGB>(sensors.getControl(), sensors.getNumControlElements()); // have to add leds in main sketch before sensors.begin()
  SensorStatus_e retval = sensors.begin();
  DEBUG_PORT.print("sensors.begin() returned: "); DEBUG_PORT.println(retval);
}

void loop() {
  SensorNode* node = sensors[0];
  node->read();

  rgb_f_t rgb;
  rgb.r = rgbElemByUI16(node->getRed());
  rgb.g = rgbElemByUI16(node->getGreen());
  rgb.b = rgbElemByUI16(node->getBlue());

  double mag = rgbToMagnitude(&rgb);
  size_t id;
  const char* name = "Unidentified";

  if(COLOR_DETECT_OK == detectedColor(&rgb, &id)){
    name = detectable_colors[id].name;
  }

  DEBUG_PORT.print("rgb: [");
  DEBUG_PORT.print(rgb.r);
  DEBUG_PORT.print(", ");
  DEBUG_PORT.print(rgb.g);
  DEBUG_PORT.print(", ");
  DEBUG_PORT.print(rgb.b);
  DEBUG_PORT.print("] ");
  DEBUG_PORT.print("mag: ");
  DEBUG_PORT.print(mag);
  DEBUG_PORT.print(" ");
  DEBUG_PORT.print("name: '");
  DEBUG_PORT.print(name);
  DEBUG_PORT.print("' ");
  DEBUG_PORT.println();

}
