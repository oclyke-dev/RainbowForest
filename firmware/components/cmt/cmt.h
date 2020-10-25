/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _RF_COMPONENT_CMT_H_
#define _RF_COMPONENT_CMT_H_

#include "Arduino.h"
#include "stdint.h"

typedef struct _column_map_t {
  uint8_t val;    // read only - detected value on the node
  uint8_t r;      // r / w - the color shown by the LED, red component
  uint8_t g;      // r / w - the color shown by the LED, green component
  uint8_t b;      // r / w - the color shown by the LED, blue component
} column_map_t;

extern column_map_t protection_map;

#endif // _RF_COMPONENT_CMT_H_
