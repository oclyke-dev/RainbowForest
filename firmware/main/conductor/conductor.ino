/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "tempo_dial.h"
#include "src/components/configuration/configuration.h"

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

#define REST_INACTIVE_SEC (60)
uint32_t rest_inactive = 0;
#define PAUSED_BLINK_PERIOD_MS (1000)

#define MAX_BPM (160)
#define MIN_BPM (60)
#define STEP_BPM (4)
#define DEF_BPM (80)
#define BPM2USPERIOD(B) (60000000/(B))

#define EXAMPLE1_PIN (2)
#define EXAMPLE2_PIN (3)
#define EXAMPLE3_PIN (4)
#define EXAMPLE4_PIN (5)
#define EXAMPLE5_PIN (6)
#define PLAYPAUSE_PIN (8)

IntervalTimer playbackTimer;
volatile bool playNext = false;
volatile bool playbackRunning = true;
volatile uint8_t playbackColumn = 0;

bool playButtonIllumination = true;

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);

  input_setup();
  tempo_setup();
}

void loop() {
  static uint32_t prev_time = 0;
  uint32_t now = millis();

  if(!playbackRunning){
    static uint32_t toggle = 0;
    if(now >= toggle){
      toggle += (PAUSED_BLINK_PERIOD_MS/2);
      playButtonIllumination != playButtonIllumination;
      digitalWrite(PLAYPAUSE_PIN, playButtonIllumination);
      DEBUG_PORT.print("paused. time (ms): ");
      DEBUG_PORT.println(millis());
    }
  }

}
