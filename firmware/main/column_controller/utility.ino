/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

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

  DEBUG_PRINTF(("Set column to %d\n", column_number));
}

void button0ISR() {
  button0_flag = true;
}
