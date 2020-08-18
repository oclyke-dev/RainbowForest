/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "configuration.h"

const color_definition_t detectable_colors[] = {
  {0.030, 1.00, 340.0, 330.0, 350.0, "Red    ", STAFF_VAL_RED},
  {0.070, 1.00, 040.0, 030.0, 050.0, "Yellow ", STAFF_VAL_YELLOW},
  {0.020, 1.00, 178.0, 170.0, 190.0, "Green  ", STAFF_VAL_GREEN},
  {0.080, 1.00, 198.0, 190.0, 210.0, "Cyan   ", STAFF_VAL_CYAN},
  {0.025, 1.00, 225.0, 215.0, 235.0, "Blue   ", STAFF_VAL_BLUE},
  {0.000, 0.05, 000.0, 000.0, 720.0, "NONE   ", STAFF_VAL_NONE},
};

bool detectedColor(const rgb_f_t* rgb, size_t* idx){
  return colorIndexByRGB(rgb, idx, detectable_colors, (sizeof(detectable_colors)/sizeof(color_definition_t)));
}