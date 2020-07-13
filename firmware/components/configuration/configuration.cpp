/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "configuration.h"

const color_definition_t detectable_colors[] = {
  {0.04, 345.0, "Red", STAFF_VAL_RED},
  {0.08, 25.0, "Yellow", STAFF_VAL_YELLOW},
  {0.03, 170.0, "Green", STAFF_VAL_GREEN},
  {0.08, 190.0, "Cyan", STAFF_VAL_CYAN},
  {0.02, 210.0, "Blue", STAFF_VAL_BLUE},
  {0.0, 0.0, "NONE", STAFF_VAL_NONE},
};

bool detectedColor(const rgb_f_t* rgb, size_t* idx){
  return colorIndexByRGB(rgb, idx, detectable_colors, (sizeof(detectable_colors)/sizeof(color_definition_t)));
}