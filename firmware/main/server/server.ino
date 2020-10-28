/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// BOARD: ESP32 Feather

#include "src/components/configuration/configuration.h"
#include "src/components/uart_bridge/uart_bridge.h"
#include "src/components/staff/staff.h"
#include "src/components/cart/cart.h"
#include "src/components/cat/cat.h"
#include "src/components/cmt/cmt.h"

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

#define CONDUCTOR_BRIDGE_PORT Serial1
#define CONDUCTOR_BRIDGE_BAUD (115200)
#define CONDUCTOR_BRIDGE_TXD 27
#define CONDUCTOR_BRIDGE_RXD 12

#define HUB_BRIDGE_PORT Serial2
#define HUB_BRIDGE_BAUD (115200)
#define HUB_BRIDGE_TXD 32
#define HUB_BRIDGE_RXD 15

UARTBridge <cart_t> conductorCartBridge(CONDUCTOR_BRIDGE_PORT);
UARTBridge <cat_t> conductorCatBridge(CONDUCTOR_BRIDGE_PORT);
UARTBridge <cart_t> hubCartBridge(HUB_BRIDGE_PORT);
UARTBridge <cat_t> hubCatBridge(HUB_BRIDGE_PORT);
cart_t cart;
cat_t cat;
Staff <column_map_t> staff;


void updateStaffValue(uint8_t col, uint8_t row, staff_data_t val){
  column_map_t prev = staff[col][row];
  prev.val = val;
  staff[col][row] = prev;
  // todo: update the web server with the new information
}

void updateStaffColor(uint8_t col, uint8_t row, uint8_t r, uint8_t g, uint8_t b){
  column_map_t prev;
  if(col < STAFF_COLS){

    if(row == COMMAND_SET_COLUMN_COLOR){
      DEBUG_PORT.println("Column Color Set Requested!");
      for(size_t row_idx = 0; row_idx < STAFF_ROWS; row_idx++){
        prev = staff[col][row_idx];
        prev.r = r;
        prev.g = g;
        prev.b = b;
        staff[col][row_idx] = prev;
      }
      // todo: update the web server with the new information
      return;
    }

    if(row < STAFF_ROWS){
      DEBUG_PORT.println("Individual Color Set Requested!");
      prev = staff[col][row];
      prev.r = r;
      prev.g = g;
      prev.b = b;
      staff[col][row] = prev;
      // todo: update the web server with the new information
      return;
    }

  }

  DEBUG_PORT.println("invalid cat received");
}

void onConductorCatReception(cat_t* cat, void* args){

  // receiving a cat message from the conductor - pass it along to the hub
  uint8_t r = ((cat->rH) << 4) | ((cat->rL) & 0x0F);              // reconstructed red component of color
  uint8_t g = ((cat->gH) << 4) | ((cat->gL) & 0x0F);              // reconstructed green component of color
  uint8_t b = ((cat->bH) << 4) | ((cat->bL) & 0x0F);              // reconstructed blue component of color

  DEBUG_PORT.print("received a cat from the conductor: {col: ");
  DEBUG_PORT.print(cat->col);
  DEBUG_PORT.print(", row: ");
  DEBUG_PORT.print(cat->row);
  DEBUG_PORT.print(", r: ");
  DEBUG_PORT.print(r);
  DEBUG_PORT.print(", g: ");
  DEBUG_PORT.print(g);
  DEBUG_PORT.print(", b: ");
  DEBUG_PORT.print(b);
  DEBUG_PORT.print("}");
  DEBUG_PORT.println();

  updateStaffColor(cat->col, cat->row, r, g, b);
  hubCatBridge.send(cat);
}

void onHubCartReception(cart_t* cart, void* args){

  DEBUG_PORT.print("received a cart from the hub: {col: ");
  DEBUG_PORT.print(cart->col);
  DEBUG_PORT.print(", row: ");
  DEBUG_PORT.print(cart->row);
  DEBUG_PORT.print(", val: ");
  DEBUG_PORT.print(cart->val);
  DEBUG_PORT.print("}");
  DEBUG_PORT.println();

  // receiving a cart message from the hub - pass it along to the conductor
  updateStaffValue(cart->col, cart->row, cart->val);
  conductorCartBridge.send(cart);
}


void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);

  CONDUCTOR_BRIDGE_PORT.begin(CONDUCTOR_BRIDGE_BAUD, SERIAL_8N1, CONDUCTOR_BRIDGE_RXD, CONDUCTOR_BRIDGE_TXD);
  HUB_BRIDGE_PORT.begin(HUB_BRIDGE_BAUD, SERIAL_8N1, HUB_BRIDGE_RXD, HUB_BRIDGE_TXD);

  conductorCatBridge.onReceive(onConductorCatReception, NULL);
  hubCartBridge.onReceive(onHubCartReception, NULL);

  staff.setSize(STAFF_COLS, STAFF_ROWS);

  DEBUG_PORT.println("rainbow forest server");

}


void loop() {
  static uint32_t prev_time = 0;
  uint32_t now = millis();

  hubCartBridge.check();
  conductorCatBridge.check();


  if((now - prev_time) > 1000){
    prev_time = now;
    DEBUG_PORT.print("rf server alive. time (ms): ");
    DEBUG_PORT.print(now);
    DEBUG_PORT.println();
  }
}