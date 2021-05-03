/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "Arduino.h"

uint8_t hexCharToNumber (char c) {
  if(c >= '0' && c <= '9'){
    return (c - '0') + 0;
  }

  if(c >= 'a' && c <= 'f'){
    return (c - 'a') + 10;
  }

  if(c >= 'A' && c <= 'F'){
    return (c - 'A') + 10;
  }

  return 0;
}

uint8_t str2u8(const char* str){
  if(!str){ return 0; }
  return (16 * hexCharToNumber(*(str + 0)) + (1 * hexCharToNumber(*(str + 1))));
}

CRGB hexToCRGB(const char* hex){
  if(!hex){ return CRGB(0,0,0); }
  
  // expects format 'xxxxxx'
  uint8_t r = str2u8(hex + 0);
  uint8_t g = str2u8(hex + 2);
  uint8_t b = str2u8(hex + 4);

  //          G  R  B (b/c that's how the leds we are using interpret the signals)
  return CRGB(g,r,b);
}

void printHex4(Stream* stream, uint32_t val){
  if(val < 0x1000){ stream->print(0); }
  if(val < 0x100){ stream->print(0); }
  if(val < 0x10){ stream->print(0); }
  stream->print(val, HEX);
}

void showColumnNumber( uint8_t c ){
  digitalWrite(LED0, (c & (1 << 0)) ? HIGH : LOW);
  digitalWrite(LED1, (c & (1 << 1)) ? HIGH : LOW);
  digitalWrite(LED2, (c & (1 << 2)) ? HIGH : LOW);
  digitalWrite(LED3, (c & (1 << 3)) ? HIGH : LOW);
}

void setColumnNumber( uint8_t c ){
  if(c >= STAFF_COLS){ return; }
  column_number = c;
  preferences.putUChar(PREFS_KEY_COL_NUM, column_number);
  showColumnNumber(column_number);

  Serial.printf("Set column to %d\n", column_number);
}

void button0ISR() {
  button0_flag = true;
}

void clearColors ( void ) {
  // read colors from this column
  DEBUG_PRINTF(("clearing node colors\n"));
  for(size_t idx = 0; idx < STAFF_ROWS; idx++){
    if(color){
      CRGB c = CRGB(0,0,0);
      if(sensors[idx]){
        *sensors[idx] = c;
      }
    }else{
      DEBUG_PRINTF(("'FAIL', "));
    }
  }
}
