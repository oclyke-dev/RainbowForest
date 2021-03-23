/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "Arduino.h"

void init_gpio (void) {
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED0, LOW);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  pinMode(BUTTON0, INPUT_PULLUP);
  attachInterrupt(BUTTON0, button0ISR, RISING);

  showColumnNumber(0xF); // show user that the board started up
  delay(250);
}

void init_staff (void) {
  // initialize the staff data containers
  staff.setDebugStream(DEBUG_PORT);
  staff.setSize(1, STAFF_ROWS);
  detection_ok_staff.setSize(1, STAFF_ROWS);
}

void init_preferences (void) {
  // get the column number
  preferences.begin(PREFS_NAMESPACE, false);
  column_number = preferences.getUChar(PREFS_KEY_COL_NUM, CONTROLLER_COLUMN);
  DEBUG_PORT.printf("Column Controller (%d)\n", column_number);
}
