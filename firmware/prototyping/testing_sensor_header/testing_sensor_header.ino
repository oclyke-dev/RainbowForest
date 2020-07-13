#include "src/components/sensor/sensor.h"


#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

#define DATA_PIN 18
#define COLUMN_LEN (7)
SensorString sensors(COLUMN_LEN, DATA_PIN);

void blinkAndRead(SensorNode* node, size_t idx, void* args){
  *(node) = CRGB(0, 127, 200);
  delay(250);
  *(node) = CRGB(0,0,0);

  node->read();

  DEBUG_PORT.print("Sensor at idx: ");
  DEBUG_PORT.print(idx);
  DEBUG_PORT.print(" read: [");
  DEBUG_PORT.print(node->getRed());
  DEBUG_PORT.print(", ");
  DEBUG_PORT.print(node->getGreen());
  DEBUG_PORT.print(", ");
  DEBUG_PORT.print(node->getBlue());
  DEBUG_PORT.print("]");
  DEBUG_PORT.println();
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);

  FastLED.addLeds<WS2811, DATA_PIN, RGB>(sensors.getControl(), sensors.getNumControlElements()); // have to add leds in main sketch before sensors.begin()
  SensorStatus_e retval = sensors.begin();
  DEBUG_PORT.print("sensors.begin() returned: "); DEBUG_PORT.println(retval);

  sensors.forEach(blinkAndRead);
  DEBUG_PORT.println();
}

void loop() {

}
