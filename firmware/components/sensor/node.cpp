/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "sensor.h"

SensorNode::SensorNode( size_t index, CRGB* control, SFE_ISL29125* sensor ) :
_index(index),
_control(control),
_sensor(sensor)
{
    _reading.r = 0;
    _reading.g = 0;
    _reading.b = 0;
}

SensorStatus_e SensorNode::_report(SensorStatus_e s){
    status = s;
    return status;
}

SensorStatus_e SensorNode::power(bool on){
    if(!_control){ return SENSOR_ERR_CONFIG; }
    _control[(2*_index) + 0] = (on) ? CRGB(0, 0, 0) : CRGB(255, 0, 0);
    FastLED.show();
    if(on){
        delay(2);   // time for sensor to wake up
    }
    return _report(SENSOR_OK);
}

isl_readint_t SensorNode::read( void ){
    // Serial.print("_index: "); Serial.println(_index);
    // Serial.print("_sensor: 0x"); Serial.println((uint32_t)_sensor, HEX);
    // Serial.print("_control: 0x"); Serial.println((uint32_t)_control, HEX);

    if((!_control) || (!_sensor)){
        _report(SENSOR_ERR_CONFIG);
        return _reading;
    }

    _setLed(CRGB(200, 200, 255));   // illuminate the target (adjust white balance)
    power(true);                    // turn on sensor power
    
    _sensor->init();

    // sensor continuously runs ADC at ~ 10 hz so to be sure wait 0.2 seconds before reading
    delay(200);
    
    delay(300); // delay to combat voltage sag from turning on all the leds...
                // i've experimentally determined that while there is no LED brightness that completely 
                // eliminates noise in detected color there is a minimum total delay between turning on
                // the leds and taking a sample that gets darn close. Its approx 500 ms total (including
                // time dedicated to letting the sensor read)

                // the final product may as well turn on all the leds, wait half a second, and then sample
                // all of the color sensors rapidly. 

    // read the sensor
    _reading.r = _sensor->readRed();
    _reading.g = _sensor->readGreen();
    _reading.b = _sensor->readBlue();

    power(false);           // turn off sensor power
    _setLed(_led);          // return led to desired color

    _report(SENSOR_OK);
    return _reading;
}

SensorStatus_e SensorNode::_setLed(const CRGB& c){
    if(!_control){ return _report(SENSOR_ERR_CONFIG); }
    _control[(2*_index) + 1] = c;
    FastLED.show();
    return _report(SENSOR_OK);
}









SensorString::SensorString(size_t len, uint32_t data_pin ) : 
_len(len),
_data_pin(data_pin)
{
    _num_control_elements = 2*_len;
    _control = new CRGB[_num_control_elements];

    _nodes = new SensorNode*[_len];
    if(!_nodes){
        _report(SENSOR_ERR_NOMEM);
        return;
    }

    for(size_t idx = 0; idx < _len; idx++){
        SensorNode* node = new SensorNode(idx, _control, &_sensor);
        if(!node){
            _report(SENSOR_ERR_NOMEM);
            return;
        }
        *(_nodes + idx) = node;
    }
}

SensorString::~SensorString(){
    if(_control){ delete _control; }
    if(_nodes){
        for(size_t idx = 0; idx < _len; idx++){
            SensorNode* node = *(_nodes + idx);
            if(node){ delete node; }
        }
        delete _nodes;
    }
}

SensorStatus_e SensorString::_report(SensorStatus_e s){
    status = s;
    return status;
}


SensorStatus_e SensorString::begin( void ){
    Serial.print("_len: "); Serial.println(_len);
    Serial.print("&_sensor: 0x"); Serial.println((uint32_t)&_sensor, HEX);
    Serial.print("_control: 0x"); Serial.println((uint32_t)_control, HEX);
    Serial.print("_nodes: 0x"); Serial.println((uint32_t)_nodes, HEX);
    Serial.print("_data_pin: "); Serial.println(_data_pin);
    Serial.print("_num_control_elements: "); Serial.println(_num_control_elements);


    if(!_control){ return _report(SENSOR_ERR_CONFIG); }
    if(!_nodes){ return _report(SENSOR_ERR_CONFIG); }

    LEDS.setBrightness(255);

    // make sure all sensors are off and not showing colors
    for(size_t idx = 0; idx < _len; idx++){
        SensorNode* node = (*this)[idx];
        if(status != SENSOR_OK){ continue; }
        node->power(false);
        *node = CRGB(0, 0, 0);
    }

    return _report(SENSOR_OK);
}
