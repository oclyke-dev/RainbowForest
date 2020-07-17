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

#define STAFF_VAL_NONE (0)
#define STAFF_VAL_RED (1)
#define STAFF_VAL_YELLOW (2)
#define STAFF_VAL_GREEN (3)
#define STAFF_VAL_CYAN (4)
#define STAFF_VAL_BLUE (5)

#define PLAYBACK_PERIOD_DEFAULT (600000) 

#define NETWORK_TCP_PORT (80)
#define NETWORK_UDP_PORT (3333)
#define NETWORK_UDP_ADDRESS "255.255.255.255"
#define NETWORK_UDP_BROADCAST_PERIOD (500)
#define NETWORK_SSID "RainbowNetESP32"
#define NETWORK_PASSWORD "ujywwjtw"

// #define DETECTABLE_COLORS_LENGTH (STAFF_VALS + 1)
extern const color_definition_t detectable_colors[];
bool detectedColor(const rgb_f_t* rgb, size_t* idx);

#endif // _RF_COMPONENT_CONFIGURATION_H_
