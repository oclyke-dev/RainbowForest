/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "configuration.h"

const color_definition_t detectable_colors[] = {
  {0.0, 0.0, "Red", STAFF_VAL_RED},
  {0.0, 60.0, "Yellow", STAFF_VAL_YELLOW},
  {0.0, 120.0, "Green", STAFF_VAL_GREEN},
  {0.0, 180.0, "Cyan", STAFF_VAL_CYAN},
  {0.0, 240.0, "Blue", STAFF_VAL_BLUE},
  {0.0, 0.0, "NONE", STAFF_VAL_NONE},
};

bool detectedColor(const rgb_f_t* rgb, size_t* idx){
  return colorIndexByRGB(rgb, idx, detectable_colors, (sizeof(detectable_colors)/sizeof(color_definition_t)));
}