/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// BOARD: Adafruit ESP32 Feather

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"

#include "src/components/configuration/configuration.h"
#include "src/components/column/column.h"
#include "src/components/cat/cat.h"

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

#define I2C_PERIPH_RX_BUF_LEN (1024)
#define I2C_PERIPH_TX_BUF_LEN (1024)

const size_t buff_len = 128;
uint8_t buff[buff_len];

void setup_buff( void ){
  memset((void*)buff, 0x00, buff_len);
}

void on_cat_received( cat_t* cat, void* params ){
  DEBUG_PORT.print("got a cat. row: ");
  DEBUG_PORT.print(cat->row);
  DEBUG_PORT.print(" time (ms): ");
  DEBUG_PORT.print(millis());
//  DEBUG_PORT.println();

  // create fake values (w/ checksum)
  column_vals_t vals;
  const size_t vals_size = (sizeof(column_vals_t)/sizeof(uint8_t));
  size_t checksum = 0;
  for(size_t idx = 0; idx < (vals_size - 1); idx++){
    *(((uint8_t*)&vals) + idx) = (uint8_t)random(0, 256);
    checksum += *(((uint8_t*)&vals) + idx);
  }
  uint8_t checksum_masked = (uint8_t)checksum;
  *(((uint8_t*)&vals) + (vals_size - 1)) = checksum_masked;

  DEBUG_PORT.print("checksum: ");
  DEBUG_PORT.print(checksum_masked);
  DEBUG_PORT.println();


  // push all the values onto the tx buffer
  if(cat->row == COMMAND_REQ_FULL_UPDATE){
    int retval = ESP_OK;
    retval = i2c_slave_write_buffer((i2c_port_t)HUB_PERIPHERAL_I2C_NUM, (uint8_t*)&vals, vals_size, 0 / portTICK_RATE_MS);
    if(retval == ESP_FAIL){
      DEBUG_PORT.println("FAILED TO WRITE OUT VALS");
    }
  }

  
}

static esp_err_t setup_i2c_peripheral(uint8_t addr)
{
    i2c_config_t conf_periph;
    conf_periph.sda_io_num = (gpio_num_t)HUB_I2C_SDA_PIN;
    conf_periph.sda_pullup_en = GPIO_PULLUP_DISABLE;
    conf_periph.scl_io_num = (gpio_num_t)HUB_I2C_SCL_PIN;
    conf_periph.scl_pullup_en = GPIO_PULLUP_DISABLE;
    conf_periph.mode = I2C_MODE_SLAVE;
    conf_periph.slave.addr_10bit_en = 0;
    conf_periph.slave.slave_addr = addr;
    i2c_param_config((i2c_port_t)HUB_PERIPHERAL_I2C_NUM, &conf_periph);
    return i2c_driver_install((i2c_port_t)HUB_PERIPHERAL_I2C_NUM, conf_periph.mode, I2C_PERIPH_RX_BUF_LEN, I2C_PERIPH_RX_BUF_LEN, 0);
}

void check_cats( void ){
  static size_t bytes_accumulated = 0;
  int bytes_read = i2c_slave_read_buffer((i2c_port_t)HUB_PERIPHERAL_I2C_NUM, buff, buff_len, 0 / portTICK_RATE_MS);

//  static uint32_t debounce = 0;
//  if(millis() > debounce){    // SW SHOULD NOT BOUNCE! THIS IS BAD
//    debounce = millis() + 10;
    
    if(bytes_read != ESP_FAIL){
      bytes_accumulated += bytes_read; // now we have some # of bytes
  
      const size_t cat_size = (sizeof(cat_t)/sizeof(uint8_t));
      if(bytes_accumulated >= cat_size){
        
        // have enough bytes to make a cat
        cat_t* cat = (cat_t*)buff;
  
        // call the user's handler
        on_cat_received(cat, NULL);
  
        // remove handled cat from buffer
        for(size_t idx = cat_size; idx < buff_len; idx++){
          buff[idx - cat_size] = buff[idx];
        }
        bytes_accumulated -= cat_size;
        
      }
      
    }else{
      Serial.print("Failed to read bytes from buffer\n");
    }
    
//  }

  

}

void setup( void ){
  DEBUG_PORT.begin(DEBUG_BAUD);

  TRISTATE_HUB_I2C_NEIGHBORS

  setup_buff();

  uint8_t col = 7; // todo: this is stored in eeprom
  if(setup_i2c_peripheral(COL_ADDR(col)) != ESP_OK){
    DEBUG_PORT.println("i2c peripheral setup failed");
    while(1){
      // lock up
    }
  }

  DEBUG_PORT.println("Column Controller - I2C Test");
  
}

void loop( void ){
  // continuously check incoming (rx) buffer for commands (either "give data" or "set color")
  check_cats();
}
