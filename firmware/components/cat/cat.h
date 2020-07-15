/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _RF_COMPONENT_CAT_H_
#define _RF_COMPONENT_CAT_H_

#include "Arduino.h"
#include "stdint.h"

typedef struct _cat_t {
    // color address type
    uint8_t col = 0;
    uint8_t row = 0;
    uint8_t rL = 0;
    uint8_t rH = 0;
    uint8_t gL = 0;
    uint8_t gH = 0;
    uint8_t bL = 0;
    uint8_t bH = 0;
} cat_t;

#endif // _RF_COMPONENT_CAT_H_
