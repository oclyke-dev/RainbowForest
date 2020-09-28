/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _RF_COMPONENT_COLUMN_H_
#define _RF_COMPONENT_COLUMN_H_

#include <stdint.h>

#define COL_ADDR(I) (0x10 + I)

typedef struct _color_rgb_t { // an rgb color structure
  uint8_t r;
  uint8_t g;
  uint8_t b;
} color_rgb_t;

typedef struct _column_colors_t { // colors for each node position, 0 - 6
  color_rgb_t c0;
  color_rgb_t c1;
  color_rgb_t c2;
  color_rgb_t c3;
  color_rgb_t c4;
  color_rgb_t c5;
  color_rgb_t c6;
} column_colors_t;

typedef struct _column_vals_t { // values for each node position, 0 - 6
  uint8_t v0;
  uint8_t v1;
  uint8_t v2;
  uint8_t v3;
  uint8_t v4;
  uint8_t v5;
  uint8_t v6;
} column_vals_t;

typedef struct _column_state_t {
  column_colors_t colors; // colors that the column is set to
  column_vals_t vals;     // values detected on the column
} columns_state_t;


#endif // _RF_COMPONENT_COLUMN_H_
