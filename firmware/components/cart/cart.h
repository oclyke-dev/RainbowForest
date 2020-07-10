/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _RF_COMPONENT_CART_H_
#define _RF_COMPONENT_CART_H_

#include "Arduino.h"
#include "stdint.h"

typedef struct _cart_t {
    // column and row transport type
    uint8_t col = 0;
    uint8_t row = 0;
    uint8_t val = 0;
} cart_t;

#endif // _RF_COMPONENT_CART_H_
