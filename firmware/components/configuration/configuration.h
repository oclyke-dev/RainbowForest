/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _RF_COMPONENT_CONFIGURATION_H_
#define _RF_COMPONENT_CONFIGURATION_H_

#include "Wire.h"
#include "color_detection.h"

#define STAFF_COLS (16)
#define STAFF_ROWS (7)
#define STAFF_VALS (5)
typedef uint8_t staff_data_t;

#define STAFF_VAL_NONE (0)
#define STAFF_VAL_RED (1)
#define STAFF_VAL_YELLOW (2)
#define STAFF_VAL_GREEN (3)
#define STAFF_VAL_CYAN (4)
#define STAFF_VAL_BLUE (5)

#define INSTRUMENT_GUITAR STAFF_VAL_RED
#define INSTRUMENT_TRUMPET STAFF_VAL_YELLOW
#define INSTRUMENT_FLUTE STAFF_VAL_GREEN
#define INSTRUMENT_DRUMS STAFF_VAL_CYAN
#define INSTRUMENT_PIANO STAFF_VAL_BLUE

#define NOTE_C (0)
#define NOTE_D (1)
#define NOTE_E (2)
#define NOTE_F (3)
#define NOTE_G (4)
#define NOTE_A (5)
#define NOTE_B (6)

#define DRUM_BASS_KICK NOTE_C
#define DRUM_HI_HAT NOTE_D


#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)
#define _WIRE_NUMBER(num) Wire##num
#define WIRE_NUMBER(num) _WIRE_NUMBER(num)

#define HUB_I2C_MODULE 1
#define HUB_PERIPHERAL_I2C_NUM I2C_NUMBER(HUB_I2C_MODULE)
#define HUB_CONTROLLER_WIRE_PORT WIRE_NUMBER(HUB_I2C_MODULE)
#define HUB_I2C_SDA_PIN (14)
#define HUB_I2C_SCL_PIN (32)

#define TRISTATE_HUB_I2C_NEIGHBORS \
  pinMode(12, INPUT); /* tri-state on the wrover footprint 14 (next to SCL1) */ \
  pinMode(27, INPUT); /* tri-state on the wrover footprint 12 (next to SCL1) */ \
  pinMode(33, INPUT); /* tri-state on the wrover footprint 6 (next to SDA1) */ \
  pinMode(35, INPUT); /* tri-state on the wrover footprint 8 (next to SDA1) (note, this is a xtal input??) */

#define COMMAND_REQ_FULL_UPDATE (STAFF_ROWS + 1)
#define COMMAND_SET_COLUMN_COLOR (STAFF_ROWS + 2)

// #define DETECTABLE_COLORS_LENGTH (STAFF_VALS + 1)
extern const color_definition_t detectable_colors[];
bool detectedColor(const rgb_f_t* rgb, size_t* idx);

#endif // _RF_COMPONENT_CONFIGURATION_H_
