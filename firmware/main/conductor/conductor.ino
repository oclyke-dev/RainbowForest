/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include <EEPROM.h>

#include "src/components/staff/staff.h"
#include "src/components/uart_bridge/uart_bridge.h"
#include "src/components/cart/cart.h"
#include "src/components/cat/cat.h"
#include "tempo_dial.h"

#include "src/components/configuration/configuration.h"

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

#define REST_INACTIVE_SEC (60)
uint32_t rest_inactive = 0;
#define PAUSED_BLINK_PERIOD_MS (1000)

#define BRIDGE_PORT Serial1
#define BRIDGE_BAUD (115200)
UARTBridge <cart_t> cartBridge(BRIDGE_PORT);
UARTBridge <cat_t> catBridge(BRIDGE_PORT);
cart_t cart;
cat_t cat;
Staff <staff_data_t> staff;

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

  BRIDGE_PORT.begin(BRIDGE_BAUD);
  cartBridge.onReceive(onCartReception, NULL);

  staff.setSize(STAFF_COLS, STAFF_ROWS);

  input_setup();
  audio_setup();
  tempo_setup();

  playbackTimer.begin(playbackISR, BPM2USPERIOD(DEF_BPM));
  
//  restore_settings();
  
  DEBUG_PORT.print("Started with settings: running: ");
  DEBUG_PORT.print(playbackRunning);
  DEBUG_PORT.println();

  requestFullUpdate();
}

void loop() {
  static uint32_t prev_time = 0;
  uint32_t now = millis();
  cartBridge.check();

  if(prev_time > now){
    sw_reset(); // handle system timer overflows the easy way
  }

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

  if(playbackRunning){
    // if((now - rest_inactive) >= (REST_INACTIVE_SEC * 1000)){
    //   rest_inactive = now;
    //   pause();
    // } 
  
    if(playNext){
      playNext = false;   
      playColumn(playbackColumn++);
      if(playbackColumn >= STAFF_COLS){
        playbackColumn = 0;
      }
    }
  }

  static uint32_t debug_staff = 0;
  if((now - debug_staff) > 100){
    debug_staff = now;
    debugShowStaff();
  }
  
}



void debugShowStaff( void ){

  for(size_t idx = 0; idx < 100; idx++){
    DEBUG_PORT.println();
  }

  for(size_t r = STAFF_ROWS; r > 0; r--){
    size_t row = r-1;
    DEBUG_PORT.print("row [");
    DEBUG_PORT.print(row);
    DEBUG_PORT.print("]: ");
    for(size_t col = 0; col < STAFF_COLS; col++){
      DEBUG_PORT.print(staff[col][row]);
      DEBUG_PORT.print(", ");
    }
    DEBUG_PORT.println();
  }
}
