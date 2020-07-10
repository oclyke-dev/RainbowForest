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

Staff <STAFF_DATA_TYPE> staff;

void onCartReception(cart_t* cart, void* args){
  staff[cart->col][cart->row] = cart->val;
  staff.showStaffDebug();
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
  
  staff[0][0] = 8;

  Serial.println(staff[0][0]);
}

void loop() {

  cartBridge.check();

}
