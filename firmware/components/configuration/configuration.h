/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _RF_COMPONENT_CONFIGURATION_H_
#define _RF_COMPONENT_CONFIGURATION_H_

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
 
#define NETWORK_MESH_PORT (5555)
#define NETWORK_UDP_PORT (3333)
#define NETWORK_UDP_ADDRESS "255.255.255.255"
#define NETWORK_UDP_BROADCAST_PERIOD (500)
#define NETWORK_SSID "RainbowNetESP32"
#define NETWORK_PASSWORD "ujywwjtw"

#define NETWORK_HUB_NAME "network_hub"
#define COLUMN_CONTROLLER_PREFIX "column_controller"

#define COMMAND_REQ_FULL_UPDATE (STAFF_ROWS + 1)
#define COMMAND_SET_COLUMN_COLOR (STAFF_ROWS + 2)

// #define DETECTABLE_COLORS_LENGTH (STAFF_VALS + 1)
extern const color_definition_t detectable_colors[];
bool detectedColor(const rgb_f_t* rgb, size_t* idx);

#endif // _RF_COMPONENT_CONFIGURATION_H_
