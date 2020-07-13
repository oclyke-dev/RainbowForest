/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _RF_COMPONENT_SENSOR_H_
#define _RF_COMPONENT_SENSOR_H_

#include <algorithm>    // std::random_shuffle
#include <cstdlib>      // std::rand, std::srand

#include "Arduino.h"
#include "Wire.h"
#include "FastLED.h"
#include "SparkFunISL29125.h"

typedef enum {
  SENSOR_OK = 0,
  SENSOR_ERR,
  SENSOR_ERR_CONFIG,
  SENSOR_ERR_NOMEM,
  SENSOR_ERR_ARGS,

  // number of status messages
  SENSOR_STATUS_NUM
} SensorStatus_e;

typedef struct _isl_reading_t {
  unsigned int r = 0;
  unsigned int g = 0;
  unsigned int b = 0;
} isl_readint_t;

class SensorNode {
private:
  SensorStatus_e _report(SensorStatus_e s);

  size_t _index;
  CRGB* _control = NULL;
  SFE_ISL29125* _sensor = NULL;

  isl_readint_t _reading;
  CRGB _led = CRGB(0, 0, 0);

  SensorStatus_e _setLed(const CRGB& c);
  
protected:
public:
  SensorNode( size_t index, CRGB* control, SFE_ISL29125* sensor );

  SensorStatus_e status = SENSOR_OK;

  isl_readint_t read( void );
  SensorStatus_e power(bool on);

  unsigned int getRed( void ){ return _reading.r; }
  unsigned int getGreen( void ){ return _reading.g; }
  unsigned int getBlue( void ){ return _reading.b; }

  void operator = (const CRGB& c){
    _led = c;
    _setLed(_led);
  }
};


typedef void(*forEach_fn)(SensorNode* node, size_t idx, void* args);

class SensorString {
private:
  size_t        _len;
  uint32_t      _data_pin;
  CRGB*         _control;
  SFE_ISL29125  _sensor;
  SensorNode**  _nodes;
  
  size_t _num_control_elements = 0;

  SensorStatus_e _report(SensorStatus_e s);

protected:
public:
  SensorString(size_t len, uint32_t data_pin);
  ~SensorString();

  SensorStatus_e status;

  SensorStatus_e begin( void );

  SensorStatus_e forEach(forEach_fn fn, void* args = NULL){
    if(!fn){ return _report(SENSOR_ERR_ARGS); }

    for(size_t idx = 0; idx < _len; idx++){
        SensorNode* node = (*this)[idx];
        if(!node){ continue; }
        fn(node, idx, args);
    }
    return _report(SENSOR_OK);
  }

  SensorStatus_e forEachRandOrder(forEach_fn fn, void* args = NULL){
    if(!fn){ return _report(SENSOR_ERR_ARGS); }

    size_t* rand_list = new size_t[_len];
    if(!rand_list){ return _report(SENSOR_ERR_NOMEM); }
    for(size_t idx = 0; idx < _len; idx++){
      rand_list[idx] = idx;
    }
    std::random_shuffle(rand_list, rand_list + _len);

    for(size_t idx = 0; idx < _len; idx++){
      size_t node_index = rand_list[idx];
      SensorNode* node = (*this)[node_index];
      if(!node){ continue; }
      fn(node, node_index, args);
    }

    delete rand_list;

    return _report(SENSOR_OK);
  }

  CRGB* getControl( void ){ return _control; }
  size_t getNumControlElements( void ){ return _num_control_elements; }

  SensorNode* operator[](size_t idx){
    if(!_nodes){ _report(SENSOR_ERR_CONFIG); return NULL;}
    SensorNode* node = *(_nodes + idx);
    if(!node){ _report(SENSOR_ERR_CONFIG); return NULL; }
    _report(SENSOR_OK);
    return node;
  }
};

#endif // _RF_COMPONENT_SENSOR_H_
