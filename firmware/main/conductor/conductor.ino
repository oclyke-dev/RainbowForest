/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// BOARD: Teensy 3.x series

#include "src/components/staff/staff.h"
#include "src/components/uart_bridge/uart_bridge.h"
#include "src/components/cart/cart.h"
#include "src/components/cat/cat.h"

#include "src/components/configuration/configuration.h"

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

#define BRIDGE_PORT Serial1
#define BRIDGE_BAUD (115200)
UARTBridge <cart_t> cartBridge(BRIDGE_PORT);
UARTBridge <cat_t> catBridge(BRIDGE_PORT);
cart_t cart;
cat_t cat;

IntervalTimer playbackTimer;
volatile bool playNext = false;
volatile bool playbackRunning = true;

Staff <staff_data_t> staff;

void randomCat( void ){
  cat.col = random(0, STAFF_COLS);
  cat.row = random(0, STAFF_ROWS);

  cat.rL = random(0, 0x10);
  cat.rH = random(0, 0x10);
  cat.gL = random(0, 0x10);
  cat.gH = random(0, 0x10);
  cat.bL = random(0, 0x10);
  cat.bH = random(0, 0x10);
}

void onCartReception(cart_t* cart, void* args){
  staff[cart->col][cart->row] = cart->val;
}

void playColumn( void ){
  if(playbackRunning){
    playNext = true;
  }
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);

  BRIDGE_PORT.begin(BRIDGE_BAUD);
  cartBridge.onReceive(onCartReception, NULL);

  staff.setDebugStream(DEBUG_PORT);
  staff.setSize(STAFF_COLS, STAFF_ROWS);

  staff.setDebugVerbose(false);
  DEBUG_PORT.print("Staff Bit Depth: ");

  playbackTimer.begin(playColumn, PLAYBACK_PERIOD_DEFAULT);
}

void loop() {
  cartBridge.check();

  if(playbackRunning && playNext){
    playNext = false;
    static uint32_t column = 0;
    DEBUG_PORT.print("Playing column: ");
    DEBUG_PORT.print(column);
    DEBUG_PORT.println();

    column++;
    if(column >= STAFF_COLS){
      column = 0;
    }
  }
}
