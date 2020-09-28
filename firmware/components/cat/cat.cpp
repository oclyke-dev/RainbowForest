/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "cat.h"

void format_cat_special_command(uint8_t command, cat_t* cat){
  cat->row = command;
}
