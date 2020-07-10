/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _RF_COMPONENT_STAFF_H_
#define _RF_COMPONENT_STAFF_H_

#include "Arduino.h"
#include "stdint.h"

typedef enum {
    STAFF_NOM = 0,
    STAFF_ERR,
    STAFF_ERR_NOMEM,
    STAFF_ERR_OUTBOUNDS,

    // number of status types
    STAFF_STATUS_NUM
} StaffStatus_e;

const char* staffStatusToString(StaffStatus_e status);

template <class StaffDataType_t> class Staff {
private:

    StaffDataType_t* _data = NULL;

protected:

    Stream* _debugs = NULL;
    bool _debug_verbose = false;
    StaffStatus_e _status = STAFF_NOM;

    uint32_t _cols = 0;
    uint32_t _rows = 0;

    StaffStatus_e report(StaffStatus_e status, const char* func = NULL, const char* file = NULL, unsigned int line = 0);
    StaffStatus_e cleanMemory( void );

public:
    
    Staff( void );
    ~Staff( void );

    StaffStatus_e setDebugStream(Stream& stream);
    StaffStatus_e setDebugVerbose(bool on);
    StaffStatus_e showStaffDebug( void );

    StaffStatus_e setSize(uint32_t cols, uint32_t rows);
    StaffStatus_e setRows(uint32_t rows);
    StaffStatus_e setCols(uint32_t cols);

    uint32_t getBitDepth( void );

    uint32_t indexOf(uint32_t col, uint32_t row);

    StaffDataType_t* operator[](uint32_t col);

};

// end specification
////////////////////




///////////////////////////
// template implementations

////////////
// protected

//
// utility functions

template <class StaffDataType_t> StaffStatus_e Staff<StaffDataType_t>::report(StaffStatus_e status, const char* func, const char* file, unsigned int line){
    _status = status;
    if(_debugs){
        if((status != STAFF_NOM) || _debug_verbose){
            _debugs->print("Staff Status: ");
            _debugs->println(staffStatusToString(_status));
            if(func){
                _debugs->print("\tFUNC: ");
                _debugs->println(func);
            }
            if(file){
                _debugs->print("\tFILE: ");
                _debugs->println(file);
                if(line){
                    _debugs->print("\tLINE: ");
                    _debugs->println(line);
                }
            }
        }
    }
    return _status;
}

template <class StaffDataType_t> StaffStatus_e Staff<StaffDataType_t>::cleanMemory( void ){
    if(_data){ delete _data; }
    return report(STAFF_NOM);
}

//
// create staff objects
template <class StaffDataType_t> Staff<StaffDataType_t>::Staff( void ){

}

template <class StaffDataType_t> Staff<StaffDataType_t>::~Staff( void ){
    cleanMemory();
}

//
// handle debugging options
template <class StaffDataType_t> StaffStatus_e Staff<StaffDataType_t>::setDebugStream(Stream& stream){
    _debugs = &stream;
    return report(STAFF_NOM);
}

template <class StaffDataType_t> StaffStatus_e Staff<StaffDataType_t>::setDebugVerbose(bool on){
    _debug_verbose = on;
    return report(STAFF_NOM);
}

template <class StaffDataType_t> StaffStatus_e Staff<StaffDataType_t>::showStaffDebug( void ){
    for(uint32_t indi = 0; indi < 300; indi++){
        _debugs->println();
    }

    for(uint32_t row = 0; row < _rows; row++){
        for(uint32_t col = 0; col < _cols; col++){
            uint32_t index = indexOf(col, row);
            _debugs->print(_data[index]);
            _debugs->print(" ");
        }
        _debugs->println();
    }
    _debugs->println("\n");
    return report(STAFF_NOM);
}

// 
// modify size of staff
template <class StaffDataType_t> StaffStatus_e Staff<StaffDataType_t>::setSize(uint32_t cols, uint32_t rows){
    StaffStatus_e status = STAFF_NOM;
    cleanMemory();
    _cols = cols;
    _rows = rows;
    _data = new StaffDataType_t [_cols * _rows];
    if(_data == NULL){
        _cols = 0;
        _rows = 0;
        status = STAFF_ERR_NOMEM;
    }
    memset((void*)_data, 0x00, (_cols * _rows)*(sizeof(StaffDataType_t)/sizeof(StaffDataType_t)));
    return report(status, __PRETTY_FUNCTION__, __FILE__, __LINE__);
}

template <class StaffDataType_t> StaffStatus_e Staff<StaffDataType_t>::setRows(uint32_t rows){
    return report(setSize(_cols, rows));
}

template <class StaffDataType_t> StaffStatus_e Staff<StaffDataType_t>::setCols(uint32_t cols){
    return report(setSize(cols, _rows));
}

template <class StaffDataType_t> uint32_t Staff<StaffDataType_t>::getBitDepth( void ){
    report(STAFF_NOM);
    return (8*(sizeof(StaffDataType_t)/sizeof(uint8_t)));
}

//
// staff access
template <class StaffDataType_t> uint32_t Staff<StaffDataType_t>::indexOf(uint32_t col, uint32_t row){
    return ((_rows * col) + row);
}

template <class StaffDataType_t> StaffDataType_t* Staff<StaffDataType_t>::operator[](uint32_t col){
    StaffStatus_e status = STAFF_NOM;
    if(col >= _cols){ 
        report(STAFF_ERR_OUTBOUNDS); 
        return (StaffDataType_t*)0;
    }
    return (StaffDataType_t*)(_data + indexOf(col, 0));
}


#endif // _RF_COMPONENT_STAFF_H_
