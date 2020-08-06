/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "configuration.h"

const color_definition_t detectable_colors[] = {
  {0.020, 350.0, "Red    ", STAFF_VAL_RED},
  {0.060, 25.0,  "Yellow ", STAFF_VAL_YELLOW},
  {0.018, 160.0, "Green  ", STAFF_VAL_GREEN},
  {0.040, 190.0, "Cyan   ", STAFF_VAL_CYAN},
  {0.014, 225.0, "Blue   ", STAFF_VAL_BLUE},
  {0.000, 0.0,   "NONE   ", STAFF_VAL_NONE},
};

bool detectedColor(const rgb_f_t* rgb, size_t* idx){
  return colorIndexByRGB(rgb, idx, detectable_colors, (sizeof(detectable_colors)/sizeof(color_definition_t)));
}