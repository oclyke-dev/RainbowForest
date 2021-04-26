/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#define BASE_DEBOUNCE_MS (250)
#define DEBOUNCE(x) (BASE_DEBOUNCE_MS + x)

#define BUTTON1_PIN (EXAMPLE1_PIN) // Example 1
#define BUTTON2_PIN (EXAMPLE2_PIN) // Example 2
#define BUTTON3_PIN (EXAMPLE3_PIN) // Example 3
#define BUTTON4_PIN (EXAMPLE4_PIN) // Example 4
#define BUTTON5_PIN (EXAMPLE5_PIN) // Example 5
#define BUTTON6_PIN (PLAYPAUSE_PIN) // Play/Pause

void input_setup( void ){
    pinMode(BUTTON1_PIN, INPUT_PULLUP);
    pinMode(BUTTON2_PIN, INPUT_PULLUP);
    pinMode(BUTTON3_PIN, INPUT_PULLUP);
    pinMode(BUTTON4_PIN, INPUT_PULLUP);
    pinMode(BUTTON5_PIN, INPUT_PULLUP);
    pinMode(BUTTON6_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(BUTTON1_PIN), isrButton1, RISING);
    attachInterrupt(digitalPinToInterrupt(BUTTON2_PIN), isrButton2, RISING);
    attachInterrupt(digitalPinToInterrupt(BUTTON3_PIN), isrButton3, RISING);
    attachInterrupt(digitalPinToInterrupt(BUTTON4_PIN), isrButton4, RISING);
    attachInterrupt(digitalPinToInterrupt(BUTTON5_PIN), isrButton5, RISING);
    attachInterrupt(digitalPinToInterrupt(BUTTON6_PIN), isrButton6, RISING);
}

void isrButton1( void ){
  static uint32_t debounce = 0;
  uint32_t now = millis();
  if(now >= debounce){
    debounce += DEBOUNCE(0);
    DEBUG_PORT.println("example song 1");
    kbSendNumeral(1);
  }
}

void isrButton2( void ){
  static uint32_t debounce = 0;
  uint32_t now = millis();
  if(now >= debounce){
    debounce += DEBOUNCE(0);
    DEBUG_PORT.println("example song 2");
    kbSendNumeral(2);
  }
}

void isrButton3( void ){
  static uint32_t debounce = 0;
  uint32_t now = millis();
  if(now >= debounce){
    debounce += DEBOUNCE(0);
    DEBUG_PORT.println("example song 3");
    kbSendNumeral(3);
  }
}

void isrButton4( void ){
  static uint32_t debounce = 0;
  uint32_t now = millis();
  if(now >= debounce){
    debounce += DEBOUNCE(0);
    DEBUG_PORT.println("example song 4");
    kbSendNumeral(4);
  } 
}

void isrButton5( void ){
  static uint32_t debounce = 0;
  uint32_t now = millis();
  if(now >= debounce){
    debounce += DEBOUNCE(0);
    DEBUG_PORT.println("example song 5");
    kbSendNumeral(5);
  }
}

void isrButton6( void ){  // play / pause
  static uint32_t debounce = 0;
  uint32_t now = millis();
  if(now >= debounce){
    debounce += DEBOUNCE(0);
    DEBUG_PORT.println("button6 (play/pause!)");
    kbSendTogglePlayback();
  }
}

void onTempoChange(TempoDial<uint8_t>* dial, void* params){
  DEBUG_PORT.println(dial->getVal()); 
}
