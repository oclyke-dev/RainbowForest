/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _RF_CONDUCTOR_TEMPO_DIAL_H_
#define _RF_CONDUCTOR_TEMPO_DIAL_H_

#include "Arduino.h"

#include <FastLED.h>
#include "src/components/configuration/configuration.h" // color conversion

#define ENC_DEBOUNCE_LEN (100)

typedef struct _encoder_state_t {
  volatile int32_t delta = 0;
  volatile int32_t total = 0;
  uint32_t A = 0;
  uint32_t B = 0;
} encoder_state_t;

extern encoder_state_t enc_state;

#define ENC_CW (+1)
#define ENC_CCW (-1)

void encoderSetup( void );
void encoderOnStep(void (*callback)(encoder_state_t*, void* ), void* args);

template <typename T> class TempoDial {
private:
  CRGB* _leds = NULL;
  size_t _leds_len = 0;

  T _val;
  T _lower;
  T _upper;
  T _step;
  void (*_colorMap)(T val, T lower, T upper, CRGB* leds, size_t len) = NULL;

  void (*_changeHandler)(TempoDial<T>* dial, void* params) = NULL;
  void* _changeHandlerParams = NULL;

  static void _enc_isr_A( void ); // because these are static (for ease of initialization) it is only possible to use one dial at a time.
  static void _enc_isr_B( void ); // this is OK for this project but should be kept in mind if this code is re used

  static void defaultColorMap(T val, T lower, T upper, CRGB* leds, size_t len);
  
protected:
public:
  size_t encA = 0;
  size_t encB = 0;
  
  TempoDial( T initial_val, T lower, T upper, T step, size_t encA, size_t encB );

  T getVal( void );
  void setVal( T val );

  void onEncoderStep( encoder_state_t* state );
  void updateLeds( void );

  void setLeds(CRGB* leds, size_t len);
  void setBounds(T lower, T upper);
  void setStepSize(T step);
  void setColorMap(void (*colorMap)(T val, T lower, T upper, CRGB* leds, size_t len));
  void setOnChange(void (*handler)(TempoDial<T>* dial, void* params), void* params = NULL);
};

extern TempoDial<uint8_t> tempo;

template <class T> void TempoDial<T>::onEncoderStep( encoder_state_t* state ){
  T prev_val = _val;
  _val += state->delta * _step;
  if(_val > _upper){
    _val = _upper;
  }
  if(_val < _lower){
    _val = _lower;
  }
  if(prev_val != _val){
    updateLeds();
    if(_changeHandler){
      _changeHandler(this, _changeHandlerParams);
    }
  }
}

template <class T> void TempoDial<T>::defaultColorMap(T val, T lower, T upper, CRGB* leds, size_t len){
  if(!leds){ return; }
  hsv_f_t hsv = {
    (((float)val - (float)lower)/((float)upper - (float)lower)) * 360.0f,
    1.0f,
    1.0f,
  };
  if(hsv.h > 360.0f){
    hsv.h = 360.0f;
  }
  if(hsv.h < 0.0f){
    hsv.h = 0.0f;
  }
  rgb_f_t rgb = hsvToRgb(&hsv);
  uint8_t r = (uint8_t)(rgb.r * 255);
  uint8_t g = (uint8_t)(rgb.g * 255);
  uint8_t b = (uint8_t)(rgb.b * 255);

  for(size_t idx = 0; idx < len; idx++){
    leds[idx] = CRGB(r, g, b);
  }
}

template <class T> void TempoDial<T>::updateLeds( void ){
  void (*mapFn)(T val, T lower, T upper, CRGB* leds, size_t len) = NULL;
  if(_colorMap != NULL){
    mapFn = _colorMap;
  }else{
    mapFn = defaultColorMap;
  }
  mapFn(_val, _lower, _upper, _leds, _leds_len);
  FastLED.show();
}

template <class T> TempoDial<T>::TempoDial( T initial_val, T lower, T upper, T step, size_t encA, size_t encB ) :
  _val(initial_val),
  _lower(lower),
  _upper(upper),
  _step(step),
  encA(encA),
  encB(encB)
{

}

template <class T> T TempoDial<T>::getVal( void ){
  return _val;
}

template <class T> void TempoDial<T>::setVal( T val ){
  _val = val;
  updateLeds();
  if(_changeHandler){
    _changeHandler(this, _changeHandlerParams);
  }
}

template <class T> void TempoDial<T>::setLeds(CRGB* leds, size_t len){
  _leds = leds;
  _leds_len = len;
}

template <class T> void TempoDial<T>::setBounds(T lower, T upper){
  _lower = lower;
  _upper = upper;
}

template <class T> void TempoDial<T>::setStepSize(T step){
  _step = step;
}

template <class T> void TempoDial<T>::setColorMap(void (*colorMap)(T val, T lower, T upper, CRGB* leds, size_t len)){
  _colorMap = colorMap;
}

template <class T> void TempoDial<T>::setOnChange(void (*handler)(TempoDial<T>*, void*), void* params){
  _changeHandler = handler;
  _changeHandlerParams = params;
}

#endif // _RF_CONDUCTOR_TEMPO_DIAL_H_
