/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "Arduino.h"


void requestColumn (void) {
  uint32_t now = millis();
  const char* fmt = \
  "{\
    \"timestamp\": \"%d\",\
    \"id\": {\
      \"name\": \"column\",\
      \"extra\": \"%d\"\
    },\
    \"request\": {\
      \"columns\": [\
        {\"column\": %d}\
      ]\
    }\
  }";

  size_t required = 1 + snprintf(NULL, 0, fmt, now, column_number, column_number);
  char msg[required] = {'\0'};
  snprintf(msg, required, fmt, now, column_number, column_number);
  private_ws.sendTXT(msg);
}


#if STAFF_ROWS == 7
#define msg_fmt \
  "{\
    \"timestamp\": \"%d\",\
    \"id\": {\
      \"name\": \"column\",\
      \"extra\": \"%d\"\
    },\
    \"update\": {\
      \"columns\": [{\
        \"column\": %d,\
        \"entries\": [\
          {\"note\": %d},\
          {\"note\": %d},\
          {\"note\": %d},\
          {\"note\": %d},\
          {\"note\": %d},\
          {\"note\": %d},\
          {\"note\": %d}\
        ]\
      }]\
    }\
  }"
void sendColumn (void) {
  uint32_t now = millis();
  size_t required = 1 + snprintf(NULL, 0, msg_fmt, now, column_number, column_number, staff[0][0], staff[0][1], staff[0][2], staff[0][3], staff[0][4], staff[0][5], staff[0][6]);
  char msg[required] = {'\0'};
  snprintf(msg, required, msg_fmt, now, column_number, column_number, staff[0][0], staff[0][1], staff[0][2], staff[0][3], staff[0][4], staff[0][5], staff[0][6]);
  private_ws.sendTXT(msg);
}
#else
#error "sendColumn(void) is only configured to work with 7 rows - please make an implementation for your number of rows"
#endif // STAFF_ROWS == 7




// // work in progress. for now use faster (but less flexible) definition above
// void sendColumn (void) {
  
//   // construct entries
//   char* entries[STAFF_ROWS] = {NULL};
//   size_t entry_lengths[STAFF_ROWS] = {0};
//   const char* entry_fmt = "{\"note\": %d}";
//   for(size_t idx = 0; idx < STAFF_ROWS; idx++){
//     staff_data_t val = staff[0][idx];
//     size_t req = 1 + snprintf(NULL, 0, entry_fmt, val);
//     char* mem = (char*)malloc(req*sizeof(char));
//     if(mem){
//       entries[idx] = mem;
//       entry_lengths[idx] = req - 1;
//       snprintf(buf, required, entry_fmt, val);
//     }
//   }

//   // assemble entries

//   const char* msg_fmt = "{\"timestamp\": \"%d\", \"id\": {\"name\": \"column\", \"extra\": \"%d\"}, \"update\": {\"entries\": [{\"column\": %d, \"row\": %d, \"entry\": {\"note\": %d}}]}}";
//   size_t required = 1 + snprintf(NULL, 0, msg_fmt, now, col, col, note, val);
//   char buf[required] = {'\0'};
//   snprintf(buf, required, msg_fmt, now, col, col, note, val);
    

//   private_ws.sendTXT(msg);

//   // clean up allocated entries
//   for(size_t idx = 0; idx < STAFF_ROWS; idx++){
//     if(entries[idx]){
//       free(entries[idx]);
//     }
//   }
// }
