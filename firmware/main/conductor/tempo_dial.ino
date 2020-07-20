/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "tempo_dial.h"

encoder_state_t enc_state;

#define DIAL_ENC_A (15)
#define DIAL_ENC_B (16)
#define DIAL_DATA  (17)
TempoDial <uint8_t> tempo(DEF_BPM, MIN_BPM, MAX_BPM, STEP_BPM, DIAL_ENC_A, DIAL_ENC_B);
#define DIAL_LEDS (8)
CRGB leds[DIAL_LEDS];

void rfColorMap(uint8_t val, uint8_t lower, uint8_t upper, CRGB* leds, size_t len){
  float percent = (((float)val - (float)lower)/((float)upper - (float)lower));
  uint8_t w_idx = round((7.0 * percent) + 0.1);
  for(size_t idx = 0; idx < w_idx; idx++){
    float x = ((float)(idx + 1)/(float)(w_idx - 1));
    float scale = pow(x, 4);
    uint8_t r = scale * 255;
    leds[idx] = CRGB(0, r, 0);
  }
  for(size_t idx = w_idx + 1; idx < len; idx++){
    float x = ((float)(idx - w_idx)/(float)(len - w_idx - 1));
    float scale = pow((1.0f - x), 3);
    uint8_t b = scale * 255;
    leds[idx] = CRGB(0, 0, b);
  }
  leds[w_idx] = CRGB(255, 255, 255);
}

void tempo_setup( void ){
  pinMode(tempo.encA, INPUT_PULLUP);
  pinMode(tempo.encB, INPUT_PULLUP);
    
  attachInterrupt(digitalPinToInterrupt(tempo.encA), _enc_isr_A, CHANGE);
  attachInterrupt(digitalPinToInterrupt(tempo.encB), _enc_isr_B, CHANGE);

  tempo.setLeds(leds, DIAL_LEDS);
  tempo.setColorMap(rfColorMap);
  FastLED.addLeds<WS2811, DIAL_DATA, RGB>(leds, DIAL_LEDS);

  tempo.setOnChange(onTempoChange);
  tempo.updateLeds();
}

void _enc_isr_A( void ){
  static uint32_t debounce = 0;
  uint32_t now = millis();
  if(now >= debounce){
    debounce = now + ENC_DEBOUNCE_LEN;
    uint8_t A = digitalRead(tempo.encA);
    uint8_t B = digitalRead(tempo.encB);
    enc_state.delta = A ? (B ? ENC_CCW : ENC_CW) : (B ? ENC_CW : ENC_CCW);
    enc_state.total += enc_state.delta;
    tempo.onEncoderStep(&enc_state);
  }
}

void _enc_isr_B( void ){
  static uint32_t debounce = 0;
  uint32_t now = millis();
  if(now >= debounce){
    debounce = now + ENC_DEBOUNCE_LEN;
    uint8_t A = digitalRead(tempo.encA);
    uint8_t B = digitalRead(tempo.encB);
    enc_state.delta = B ? (A ? ENC_CW : ENC_CCW) : (A ? ENC_CCW : ENC_CW);
    enc_state.total += enc_state.delta;
    tempo.onEncoderStep(&enc_state);
  }
}
