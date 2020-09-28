/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// BOARD: Adafruit ESP32 Feather

#include "src/components/configuration/configuration.h"
#include "src/components/column/column.h"
#include "src/components/cat/cat.h"

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

#define COL_IDX (0) // temp

cat_t cat; // to send commands to the columns

esp_err_t test_column_transmission(uint8_t col_idx){
  esp_err_t stat = ESP_OK;
  column_vals_t vals;
  const size_t vals_size = (sizeof(column_vals_t)/sizeof(uint8_t));
  memset((void*)&vals, 0x00, vals_size);
  stat = get_column_vals(col_idx, &vals);
  if(stat != ESP_OK){
    DEBUG_PORT.print("val fail");
    return ESP_FAIL;
  }else{
    // check the checksum of the values (simple validation)
    size_t checksum = 0;
    for(size_t idx = 0; idx < (vals_size - 1); idx++){
      checksum += *(((uint8_t*)&vals) + idx);
    }
    // check checksum
    uint8_t checksum_got = *(((uint8_t*)&vals) + (vals_size - 1));
    uint8_t checksum_masked = (uint8_t)checksum;
    if(checksum_masked != checksum_got){
      DEBUG_PORT.print("checksum fail");
//      DEBUG_PORT.print(" (got: ");
//      DEBUG_PORT.print(checksum_got);
//      DEBUG_PORT.print(", expected: ");
//      DEBUG_PORT.print(checksum_masked);
      return ESP_FAIL;
    }else{
//      DEBUG_PORT.println("Vals RX'd OK");
    }
  }

  return ESP_OK;
}

esp_err_t get_column_vals(uint8_t idx, column_vals_t* vals){
  // request the column to put data in the buffer
  format_cat_special_command(COMMAND_REQ_FULL_UPDATE, &cat);
  HUB_CONTROLLER_WIRE_PORT.beginTransmission(COL_ADDR(idx));
  HUB_CONTROLLER_WIRE_PORT.write((uint8_t*)(&cat), (sizeof(cat_t)/sizeof(uint8_t))); // send entire cat
  HUB_CONTROLLER_WIRE_PORT.endTransmission();

  delay(60); // need a delay?? - for peripheral to respond and put data in buffer...??
 
  const size_t expected = (sizeof(column_vals_t)/sizeof(uint8_t));
  size_t recieved = HUB_CONTROLLER_WIRE_PORT.requestFrom(COL_ADDR(idx), expected);
//  DEBUG_PORT.printf("expected: %d, recieved: %d ", expected, recieved);
  if(expected != recieved){
    while(HUB_CONTROLLER_WIRE_PORT.available()){
      HUB_CONTROLLER_WIRE_PORT.read(); // clear rx buffer
    }
    return ESP_FAIL;
  }

  uint8_t* vals_ui8 = (uint8_t*)vals;
  for(size_t idx = 0; idx < expected; idx++){
    *(vals_ui8 + idx) = HUB_CONTROLLER_WIRE_PORT.read();
  }

  return ESP_OK;
}

void setup( void ){
  DEBUG_PORT.begin(DEBUG_BAUD);

  DEBUG_PORT.println("Network Hub - I2C Test");

  TRISTATE_HUB_I2C_NEIGHBORS
  HUB_CONTROLLER_WIRE_PORT.begin(HUB_I2C_SDA_PIN, HUB_I2C_SCL_PIN);
}

void loop( void ){

  DEBUG_PORT.print("testing columns: [");
  for(size_t idx = 0; idx < STAFF_COLS; idx++){
    DEBUG_PORT.print("(");
    DEBUG_PORT.print(idx);
    DEBUG_PORT.print(") ");

    if(test_column_transmission(idx) != ESP_OK){
//      DEBUG_PORT.print("FAIL");
    }else{
      DEBUG_PORT.print("OK");
    }

    DEBUG_PORT.print(", ");
  }
  DEBUG_PORT.println();
  
  delay(20);
}
