/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// for use with Teensy boards

void kbSendStr(const char* str) {
  Keyboard.print(str);
}

void kbPush( uint32_t key){
  Keyboard.press(key);
  Keyboard.release(key);
}

void kbSendNumeral( uint8_t val ){
  switch( val ){
    case 0: kbPush(KEYPAD_0); break;
    case 1: kbPush(KEYPAD_1); break;
    case 2: kbPush(KEYPAD_2); break;
    case 3: kbPush(KEYPAD_3); break;
    case 4: kbPush(KEYPAD_4); break;
    case 5: kbPush(KEYPAD_5); break;
    case 6: kbPush(KEYPAD_6); break;
    case 7: kbPush(KEYPAD_7); break;
    case 8: kbPush(KEYPAD_8); break;
    case 9: kbPush(KEYPAD_9); break;
    default: 
      break;
  }
}

void kbSendVolumeUp( void ){
  kbPush(KEY_MEDIA_VOLUME_INC);
}

void kbSendVolumeDown( void ){
  kbPush(KEY_MEDIA_VOLUME_DEC);
}

void kbSendTempoUp( void ){
  kbPush(KEY_UP);
}

void kbSendTempoDown( void ){
  kbPush(KEY_DOWN);
}

void kbSendTogglePlayback( void ){
  kbPush(KEY_MEDIA_PLAY_PAUSE);
}


