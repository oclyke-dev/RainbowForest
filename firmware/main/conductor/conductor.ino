/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "src/components/staff/staff.h"
#include "src/components/uart_bridge/uart_bridge.h"
#include "src/components/cart/cart.h"

#include "src/components/configuration/configuration.h"

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)
#define DEBUG_VERBOSE (true)

#define BRIDGE_PORT Serial1
#define BRIDGE_BAUD (115200)
UARTBridge <cart_t> cartBridge(BRIDGE_PORT);
cart_t cart;

IntervalTimer playbackTimer;
bool playbackRunning = true;

typedef uint8_t staff_data_t;
Staff <staff_data_t> staff;

void onCartReception(cart_t* cart, void* args){
  staff[cart->col][cart->row] = cart->val;
//  staff.showStaffDebug();
}

void playColumn( void ){
  //make player in here
  random_cart();
  
  static volatile uint32_t playbackColumn = 0;
  staff[playbackColumn][cart.row] = cart.val;
  if(playbackRunning){
    staff_data_t* data = staff[playbackColumn];
  
    for(uint32_t indi = 0; indi < STAFF_ROWS; indi++){
      DEBUG_PORT.print(*(data + indi));
      DEBUG_PORT.print(" ");
    }
    DEBUG_PORT.println();
  
    playbackColumn++;
    if(playbackColumn >= STAFF_COLS){
      playbackColumn = 0;
    }
  }
}

void random_cart( void ){
  cart.col = random(0, STAFF_COLS);
  cart.row = random(0, STAFF_ROWS);
  cart.val = random(0, STAFF_VALS);
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
  if(DEBUG_VERBOSE){ while(!DEBUG_PORT){}; }

  BRIDGE_PORT.begin(BRIDGE_BAUD);
  cartBridge.onReceive(onCartReception, NULL);

  staff.setDebugVerbose(DEBUG_VERBOSE);
  staff.setDebugStream(DEBUG_PORT);
  staff.setSize(STAFF_COLS, STAFF_ROWS);

  staff.setDebugVerbose(false);
  DEBUG_PORT.print("Staff Bit Depth: ");
  DEBUG_PORT.println(staff.getBitDepth());
  staff.setDebugVerbose(DEBUG_VERBOSE);

  playbackTimer.begin(playColumn, PLAYBACK_PERIOD_DEFAULT);
}

void loop() {

  cartBridge.check();

}
