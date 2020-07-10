/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "src/components/uart_bridge/uart_bridge.h"
#include "src/components/cart/cart.h"

#include "src/components/configuration/configuration.h"

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

#define BRIDGE_PORT Serial1
#define BRIDGE_BAUD (115200)
UARTBridge <cart_t> cartBridge(BRIDGE_PORT);
cart_t cart;

void random_cart( void ){
  cart.col = random(0, STAFF_COLS);
  cart.row = random(0, STAFF_ROWS);
  cart.val = random(0, STAFF_VALS);
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
  BRIDGE_PORT.begin(BRIDGE_BAUD);
}

void loop() {
  random_cart();
  cartBridge.send(&cart);
  delay(200);
}
