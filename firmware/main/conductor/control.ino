/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#define EEPROM_VALID_KEY (0xE3)

void pause( void ){
  setPlaybackRunning( false );
  playButtonIllumination = false;
  digitalWrite(PLAYPAUSE_PIN, playButtonIllumination);
}

void play( void ){
  playbackColumn = 0;
  playNext = false;
  setPlaybackRunning( true );
  playButtonIllumination = true;
  digitalWrite(PLAYPAUSE_PIN, playButtonIllumination);
}

void sw_reset( void ){
  // NVIC_SystemReset(void);
  SCB_AIRCR = 0x05FA0004;
}

void playbackISR( void ){
  playNext = true;
}

void setBPM( uint8_t bpm ){
  playbackTimer.update(BPM2USPERIOD(bpm));
  if(bpm != tempo.getVal()){
    tempo.setVal(bpm);
  }
  EEPROM.write(2, bpm);
  DEBUG_PORT.print("tempo set to: ");
  DEBUG_PORT.print(bpm);
  DEBUG_PORT.println(" bpm");
}

void setPlaybackRunning( bool run ){
  playbackRunning = run;
  EEPROM.write(1, (uint8_t)playbackRunning);
  DEBUG_PORT.print("playbackRunning set to: ");
  DEBUG_PORT.println(playbackRunning);
}

void restore_settings( void ){
  uint8_t eeprom_configured = EEPROM.read(0);
  if(eeprom_configured == EEPROM_VALID_KEY){
    setPlaybackRunning(EEPROM.read(1));
    setBPM(EEPROM.read(2));
  }else{
    EEPROM.write(0, EEPROM_VALID_KEY);
    EEPROM.write(1, (uint8_t)playbackRunning);
    EEPROM.write(2, DEF_BPM);
  }
}

void onCartReception(cart_t* cart, void* args){
  staff_data_t prev = staff[cart->col][cart->row];
  if(prev != cart->val){
    staff[cart->col][cart->row] = cart->val;
    rest = millis() + (REST_INACTIVE_SEC * 1000);
  }
}

void setColumnHSV( CHSV hsv, staff_data_t col){
  CRGB rgb;
  hsv2rgb_rainbow( hsv, rgb);
  
  setColumnRGB(rgb, col);
}

void setColumnRGB( CRGB rgb, staff_data_t col){ 
  cat.col = col;
  cat.row = COMMAND_SET_COLUMN_COLOR;
  cat.rH = (rgb.r >> 4);
  cat.rL = (rgb.r & 0x0F);
  cat.gH = (rgb.g >> 4);
  cat.gL = (rgb.g & 0x0F);
  cat.bH = (rgb.b >> 4);
  cat.bL = (rgb.b & 0x0F);
  catBridge.send(&cat);
}

void setRGB( CRGB rgb, staff_data_t row, staff_data_t col ){
  cat.col = col;
  cat.row = row;
  cat.rH = (rgb.r >> 4);
  cat.rL = (rgb.r & 0x0F);
  cat.gH = (rgb.g >> 4);
  cat.gL = (rgb.g & 0x0F);
  cat.bH = (rgb.b >> 4);
  cat.bL = (rgb.b & 0x0F);
  catBridge.send(&cat);
}

void requestFullUpdate( void ){
  cat.row = COMMAND_REQ_FULL_UPDATE;
  catBridge.send(&cat);
}
