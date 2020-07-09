/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "src/components/staff/staff.h"

Staff <uint8_t> staff;
#define STAFF_COLS (16)
#define STAFF_ROWS (7)

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)
#define DEBUG_VERBOSE (true)



void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
  if(DEBUG_VERBOSE){ while(!DEBUG_PORT){}; }

  staff.setDebugVerbose(DEBUG_VERBOSE);
  staff.setDebugStream(DEBUG_PORT);
  staff.setSize(STAFF_COLS, STAFF_ROWS);

  staff.setDebugVerbose(false);
  DEBUG_PORT.print("Staff Bit Depth: ");
  DEBUG_PORT.println(staff.getBitDepth());
  staff.setDebugVerbose(DEBUG_VERBOSE);

}

void loop() {

}
