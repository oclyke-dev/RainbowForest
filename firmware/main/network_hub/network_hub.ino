/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// BOARD: SparkFun SAMD21 Mini Breakout

#include "src/components/configuration/configuration.h"
#include "src/components/uart_bridge/uart_bridge.h"
#include "src/components/staff/staff.h"
#include "src/components/cart/cart.h"
#include "src/components/cat/cat.h"
#include "src/components/cmt/cmt.h"

#include <Wire.h>
#include "wiring_private.h" // pinPeripheral() function

#define LED0 A0
#define LED1 A3
#define LED2 8
#define LED3 9

#define BUTTON0 10
#define BUTTON1 12

#define RAND_PIN A1

#define DEBUG_PORT SerialUSB
#define DEBUG_BAUD (115200)

#define BRIDGE_PORT Serial1
#define BRIDGE_BAUD (115200)
UARTBridge <cart_t> cartBridge(BRIDGE_PORT);
UARTBridge <cat_t> catBridge(BRIDGE_PORT);
cart_t cart;
cat_t cat;
Staff <staff_data_t> staff;

#define NETWORK_SDA_PIN 11
#define NETWORK_SCL_PIN 13
#define NETWORK_WIRE_IT_HANDLER SERCOM1_Handler
TwoWire NetworkWire(&sercom1, NETWORK_SDA_PIN, NETWORK_SCL_PIN);
#define NETWORK_ADDRESS_BY_COL(c) (c + 0x04)

volatile bool cat_received = false;
volatile bool cleanup_cat_received = false;
volatile bool full_update_requested = true;
bool full_update_sent = false;

volatile bool button0 = false;
void button0ISR() {
  button0 = true;
}

volatile bool button1 = false;
void button1ISR() {
  button0 = true;
}

void onCatReception(cat_t* cat, void* args){
  cat_received = true;

  if(cat->col >= STAFF_COLS){
    DEBUG_PORT.println("received an invalid cat");
    return;
  }

  size_t node_data_size = (sizeof(column_map_t)/sizeof(uint8_t)); // note: if node_data_size * STAFF_ROWS > 32 you will need to use separate transmissions!
  uint8_t r = ((cat->rH) << 4) | ((cat->rL) & 0x0F);              // reconstructed red component of color
  uint8_t g = ((cat->gH) << 4) | ((cat->gL) & 0x0F);              // reconstructed green component of color
  uint8_t b = ((cat->bH) << 4) | ((cat->bL) & 0x0F);              // reconstructed blue component of color

  DEBUG_PORT.print("conductor --> column[");
  DEBUG_PORT.print(cat->col);
  DEBUG_PORT.print("]");
  DEBUG_PORT.print(", {");
  DEBUG_PORT.print(" col: ");
  DEBUG_PORT.print(cat->col);
  DEBUG_PORT.print(", row: ");
  DEBUG_PORT.print(cat->row);
  DEBUG_PORT.print(", r: ");
  DEBUG_PORT.print(r);
  DEBUG_PORT.print(", g: ");
  DEBUG_PORT.print(g);
  DEBUG_PORT.print(", b: ");
  DEBUG_PORT.print(b);
  DEBUG_PORT.print(" }");
  DEBUG_PORT.println();

  if(cat->row == COMMAND_REQ_FULL_UPDATE){
    DEBUG_PORT.println("Full Update Requested!");
    full_update_requested = true;
    return;
  }

  if(cat->row == COMMAND_SET_COLUMN_COLOR){
    DEBUG_PORT.println("Column Color Set Requested!");
    NetworkWire.beginTransmission(NETWORK_ADDRESS_BY_COL(cat->col));
    NetworkWire.write(0);                                             // begin at register 0
    for(size_t node_idx = 0; node_idx < STAFF_ROWS; node_idx++){
      NetworkWire.write(0x00);                                        // dummy write to the read-only value reg
      NetworkWire.write(r);                                           // write red
      NetworkWire.write(g);                                           // write green
      NetworkWire.write(b);                                           // write blue
    }
    NetworkWire.endTransmission();
    return;
  }

  if((cat->col < STAFF_COLS) && (cat->row < STAFF_ROWS)){
    DEBUG_PORT.println("Individual Color Set Requested!");
    NetworkWire.beginTransmission(NETWORK_ADDRESS_BY_COL(cat->col));
    NetworkWire.write(node_data_size * cat->row);                      // begin at proper offset for row
    NetworkWire.write(0x00);                                          // dummy write to the read-only value reg
    NetworkWire.write(r);                                             // write red
    NetworkWire.write(g);                                             // write green
    NetworkWire.write(b);                                             // write blue
    NetworkWire.endTransmission();
    return;
  }
  
  DEBUG_PORT.println("Unrecognized CAT...");
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
  BRIDGE_PORT.begin(BRIDGE_BAUD);

  pinMode(BUTTON0, INPUT_PULLUP);
  pinMode(BUTTON1, INPUT_PULLUP);
  attachInterrupt(BUTTON0, button0ISR, RISING);
  attachInterrupt(BUTTON1, button1ISR, RISING);

  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  digitalWrite(LED0, HIGH);
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);

  delay(250);

  digitalWrite(LED0, LOW);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  staff.setSize(STAFF_COLS, STAFF_ROWS);

  NetworkWire.begin();
  pinPeripheral(NETWORK_SDA_PIN, PIO_SERCOM);
  pinPeripheral(NETWORK_SCL_PIN, PIO_SERCOM);
  
  catBridge.onReceive(onCatReception, NULL);

  // prevent user input until setup is complete
  button0 = false;
  button1 = false;
}

void loop() {
  sampleColumns();

  catBridge.check();
  if(button0){
    DEBUG_PORT.println("button0 released");
    button0 = false;
  }

  if(cat_received){
    digitalWrite(LED1, HIGH);
    cat_received = false;
    cleanup_cat_received = true;
  }
  if(cleanup_cat_received){
    cleanup_cat_received = false;
    digitalWrite(LED1, LOW);
  }

  // alive heartbeat
  static bool led0_val = false;
  digitalWrite(LED0, led0_val);
  led0_val = !led0_val;

  // static uint8_t col_to_set = 0;
  // if(DEBUG_PORT.available()){
  //   DEBUG_PORT.read();
  //   cat.row = COMMAND_SET_COLUMN_COLOR;
  //   cat.col = col_to_set;
  //   cat.rH = 0x00;
  //   cat.rL = 0x00;
  //   cat.bH = 0x00;
  //   cat.bL = 0x00;
  //   cat.gH = 0x00;
  //   cat.gL = 0x00;
  //   onCatReception(&cat, NULL);
  //   col_to_set++;
  //   if(col_to_set >= STAFF_COLS){
  //     col_to_set = 0;
  //   }
  // }

  // uint32_t now = millis();
  // static uint32_t debug_staff = 0;
  // if((now - debug_staff) > 100){
  //   debug_staff = now;
  //   debugShowStaff();
  // }
}

void sampleColumns( void ){
  size_t node_data_size = (sizeof(column_map_t)/sizeof(uint8_t));
  size_t bytes_per_column = node_data_size*STAFF_ROWS;                // note: if node_data_size * STAFF_ROWS > 32 you will need to use separate transmissions!
  staff_data_t prev = 0x00;

  full_update_sent = full_update_requested; // ensure that when a full update is requested at least one full cycle is captured
  
  bool success = true;

  // sample through all the positions, if there is a change send it as a cart message to the conductor
  for(size_t col = 0; col < STAFF_COLS; col++){
    NetworkWire.beginTransmission(NETWORK_ADDRESS_BY_COL(col));
    NetworkWire.write(0);
    NetworkWire.endTransmission(false);
    size_t bytes_received = NetworkWire.requestFrom(NETWORK_ADDRESS_BY_COL(col), bytes_per_column);
    if(bytes_received != bytes_per_column){
      while(NetworkWire.available()){ NetworkWire.read(); } // dump any straggler bytes
      success = false;
      DEBUG_PORT.print("error reading data from column: ");
      DEBUG_PORT.print(col);
      DEBUG_PORT.print(". time (ms): ");
      DEBUG_PORT.print(millis());
      DEBUG_PORT.println();
      continue;                                             // continue to the next column (big failure)
    }

    for(size_t row = 0; row < STAFF_ROWS; row++){
      staff_data_t current = NetworkWire.read();            // get the value for this row position
      NetworkWire.read(); // r  (read out the color bytes so that the next row's value is next in line)
      NetworkWire.read(); // g
      NetworkWire.read(); // b

      prev = staff[col][row];
      staff[col][row] = current;

//      if(1){ // temporary
       if((prev != current) || (full_update_requested)){     // send the current value if the value at this position has changed or a full update is requested
      
        cart.col = col;
        cart.row = row;
        cart.val = current;

        DEBUG_PORT.print("column[");
        DEBUG_PORT.print(cart.col);
        DEBUG_PORT.print("]");
        DEBUG_PORT.print(" --> conductor , {");
        DEBUG_PORT.print(" col: ");
        DEBUG_PORT.print(cart.col);
        DEBUG_PORT.print(", row: ");
        DEBUG_PORT.print(cart.row);
        DEBUG_PORT.print(", val: ");
        DEBUG_PORT.print(cart.val);
        DEBUG_PORT.print(" }");
        DEBUG_PORT.println();

        cartBridge.send(&cart);
      }
    }
  }

//  onCatReception(&cat, NULL); // use this to re-send the current cat (increased sends means more likely to remedy a failure)

  if(full_update_sent){
    full_update_sent = false;
    full_update_requested = false;
  }

  if(success){
    digitalWrite(LED3, HIGH);
  }else{
    digitalWrite(LED3, LOW);
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
