/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _RF_COMPONENT_UART_BRIDGE_H_
#define _RF_COMPONENT_UART_BRIDGE_H_

#include "Arduino.h"
#include "stdint.h"

typedef enum {
    BRIDGE_OK = 0,
    BRIDGE_ERR,
    BRIDGE_ERR_ARGS,
    BRIDGE_ERR_DATA,
    BRIDGE_ERR_HEIGHT,
    BRIDGE_ERR_TRANSPORT,
    BRIDGE_ERR_UNDERFLOW,
    BRIDGE_ERR_CORRUPTED,

    // number of status elements
    BRIDGE_STATUS_NUM
} UARTBridgeStatus_t;

template <class UARTBridgeDataType_t> class UARTBridge {
private:

    Stream* _stream = NULL;
    uint8_t _div = 0;
    uint8_t _height = 0x20;

    UARTBridgeStatus_t resetDiv( void );

protected:

    void (*_receptionCallback)(UARTBridgeDataType_t* val, void* args);
    void* _receptionArgs = NULL;

public:

    UARTBridge(Stream& stream);

    UARTBridgeStatus_t setHeight(uint8_t height);
    UARTBridgeStatus_t onReceive(void (*callback)(UARTBridgeDataType_t* val, void* args), void* args = NULL);

    UARTBridgeStatus_t send(UARTBridgeDataType_t* cart);
    UARTBridgeStatus_t check( void );

    UARTBridgeStatus_t writeData(uint8_t datum);
    UARTBridgeStatus_t writeData(uint8_t* buf, uint32_t len);
    UARTBridgeStatus_t writeControl(uint8_t signal);
    UARTBridgeStatus_t write(uint8_t val, bool clearance);
    UARTBridgeStatus_t write(uint8_t* buf, uint32_t len, bool clearance);

    uint8_t read(bool accumulate = true);
};

//
// utilities
template <class UARTBridgeDataType_t> UARTBridgeStatus_t UARTBridge<UARTBridgeDataType_t>::resetDiv( void ){
    _div = 0;
    return BRIDGE_OK;
}

//
// instantiate objects
template <class UARTBridgeDataType_t> UARTBridge<UARTBridgeDataType_t>::UARTBridge(Stream& stream){
    _stream = &stream;
}

//
// configure
template <class UARTBridgeDataType_t> UARTBridgeStatus_t UARTBridge<UARTBridgeDataType_t>::setHeight(uint8_t height){
    _height = height;
    return BRIDGE_OK;
}

template <class UARTBridgeDataType_t> UARTBridgeStatus_t UARTBridge<UARTBridgeDataType_t>::onReceive(void (*callback)(UARTBridgeDataType_t* val, void* args), void* args){
    _receptionCallback = callback;
    _receptionArgs = args;
    return BRIDGE_OK;
}

//
// send and receive
template <class UARTBridgeDataType_t> UARTBridgeStatus_t UARTBridge<UARTBridgeDataType_t>::send(UARTBridgeDataType_t* cart){
    if(!cart){ return BRIDGE_ERR_ARGS; }

    UARTBridgeStatus_t status = BRIDGE_OK;

    if(resetDiv()                                                                   != BRIDGE_OK){ status = BRIDGE_ERR; };
    if(writeControl(_height + 0x00)                                                 != BRIDGE_OK){ status = BRIDGE_ERR; };
    if(writeData((uint8_t*)cart, (sizeof(UARTBridgeDataType_t)/sizeof(uint8_t)))    != BRIDGE_OK){ status = BRIDGE_ERR; };
    if(writeControl(_height + 0x01)                                                 != BRIDGE_OK){ status = BRIDGE_ERR; };
    if(writeControl(_div)                                                           != BRIDGE_OK){ status = BRIDGE_ERR; };
    
    return status;
}

template <class UARTBridgeDataType_t> UARTBridgeStatus_t UARTBridge<UARTBridgeDataType_t>::check( void ){
    if(!_stream){ return BRIDGE_ERR_TRANSPORT; }

    UARTBridgeStatus_t status = BRIDGE_OK;
    UARTBridgeDataType_t cart;

    const uint32_t data_len = (sizeof(UARTBridgeDataType_t)/sizeof(uint8_t));
    uint8_t* pdata = (uint8_t*)&cart;

    // Serial.println(_stream->available());

    if(_stream->available() < (data_len + 3)){ return BRIDGE_ERR_UNDERFLOW; }
    
    resetDiv();

    if(read() != (_height + 0x00)){ return BRIDGE_ERR_CORRUPTED; }

    for(uint32_t indi = 0; indi < data_len; indi++){
        pdata[indi] = read();
    }

    if(read() != (_height + 0x01)){ return BRIDGE_ERR_CORRUPTED; }
    if(read(false) != _div){ return BRIDGE_ERR_CORRUPTED; }

    if(_receptionCallback){
        _receptionCallback(&cart, _receptionArgs);
    }

    return status;
}


//
// write data or control characters
template <class UARTBridgeDataType_t> UARTBridgeStatus_t UARTBridge<UARTBridgeDataType_t>::writeData(uint8_t datum){
    return write(&datum, 1, false);
}

template <class UARTBridgeDataType_t> UARTBridgeStatus_t UARTBridge<UARTBridgeDataType_t>::writeData(uint8_t* buf, uint32_t len){
    return write(buf, len, false);
}

template <class UARTBridgeDataType_t> UARTBridgeStatus_t UARTBridge<UARTBridgeDataType_t>::writeControl(uint8_t signal){
    return write(&signal, 1, true);
}


template <class UARTBridgeDataType_t> UARTBridgeStatus_t UARTBridge<UARTBridgeDataType_t>::write(uint8_t datum, bool clearance){
    return write(&datum, 1, clearance);
}

template <class UARTBridgeDataType_t> UARTBridgeStatus_t UARTBridge<UARTBridgeDataType_t>::write(uint8_t* buf, uint32_t len, bool clearance){
    if(!_stream){ return BRIDGE_ERR_TRANSPORT; }
    if(!buf){ return BRIDGE_ERR_ARGS; }

    UARTBridgeStatus_t status = BRIDGE_OK;

    for(uint32_t indi = 0; indi < len; indi++){
        uint8_t val = *(uint8_t*)(buf + indi);
        if((!clearance) && (val >= _height)){
            val = 0;
            status = BRIDGE_ERR_HEIGHT;
        }
        _div += val;
        _stream->write(val);
    }
    return status;
}

//
// read
template <class UARTBridgeDataType_t> uint8_t UARTBridge<UARTBridgeDataType_t>::read( bool accumulate ){
    uint8_t val = _stream->read();
    if(accumulate){ _div += val; }
    return val;
}

#endif // _RF_COMPONENT_UART_BRIDGE_H_
