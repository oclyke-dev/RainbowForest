/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "configuration.h"

const color_definition_t detectable_colors[] = {
  {0.015, 1.00, 340.0, 325.0, 355.0, "Red    ", STAFF_VAL_RED},
  {0.035, 1.00, 040.0, 020.0, 060.0, "Yellow ", STAFF_VAL_YELLOW},
  {0.015, 1.00, 178.0, 140.0, 190.0, "Green  ", STAFF_VAL_GREEN},
  {0.020, 1.00, 198.0, 190.0, 210.0, "Cyan   ", STAFF_VAL_CYAN},
  {0.015, 1.00, 225.0, 215.0, 235.0, "Blue   ", STAFF_VAL_BLUE},
  {0.000, 0.10, 135.0, 000.0, 720.0, "NONE   ", STAFF_VAL_NONE},
};

bool detectedColor(const rgb_f_t* rgb, size_t* idx){
  return colorIndexByRGB(rgb, idx, detectable_colors, (sizeof(detectable_colors)/sizeof(color_definition_t)));
}

const char* name_by_val_error = "error getting name by value!";
const char* getColorNameByStaffValue(staff_data_t val){
  const char* retval = NULL;
  size_t num_detectable_colors = (sizeof(detectable_colors)/sizeof(color_definition_t));
  size_t color_index = 0;
  for(color_index; color_index < num_detectable_colors; color_index++){
    if(detectable_colors[color_index].staff_val == val){
      retval = detectable_colors[color_index].name;
      break;
    }
  }
  if(color_index >= num_detectable_colors){
    retval = name_by_val_error;
  }
  return retval;
}
