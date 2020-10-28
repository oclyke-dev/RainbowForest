/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

const size_t rolling_avg_len = 16;
double mag_history[rolling_avg_len];

void printHex4(Stream* stream, uint32_t val){
  if(val < 0x1000){ stream->print(0); }
  if(val < 0x100){ stream->print(0); }
  if(val < 0x10){ stream->print(0); }
  stream->print(val, HEX);
}

void detectAndTransmit(SensorNode* node, size_t idx, void* args){
  bool force_update = *((bool*)args);

  node->read();
  
  rgb_f_t rgb;
  rgb.r = rgbElemByUI16(node->getRed());
  rgb.g = rgbElemByUI16(node->getGreen());
  rgb.b = rgbElemByUI16(node->getBlue());

  prev_staff[0][idx] = staff[0][idx]; // store last staff value

  // for normal operation detect colors based on sensor results
  size_t color_index;
  if(COLOR_DETECT_OK == detectedColor(&rgb, &color_index)){
    staff_data_t val = detectable_colors[color_index].staff_val;
    detection_ok_staff[0][idx] = true;
    staff[0][idx] = (staff_data_t)val;
    if((staff[0][idx] != prev_staff[0][idx]) || (force_update)){
      setRowValue(idx, staff[0][idx]);
    }
  }else{
    detection_ok_staff[0][idx] = false;
  }

//  // for debugging generate random data
//  staff[0][idx] = random(0, STAFF_VALS + 1);
//  detection_ok_staff[0][idx] = true;
//  if((staff[0][idx] != prev_staff[0][idx]) || (force_update)){
//    setRowValue(idx, staff[0][idx]);
//  }

  delay(random(64));
}

void printInfo(SensorNode* node, size_t idx, void* args){
  bool force_update = *((bool*)args);
  
  DEBUG_PORT.print(idx);
  DEBUG_PORT.print(": ");

  staff_data_t current = staff[0][idx];
  if(detection_ok_staff[0][idx]){
    if((current != prev_staff[0][idx]) || (force_update)){
      DEBUG_PORT.print("(X) ");
      if(force_update){
        DEBUG_PORT.print("(F) ");
      }
    }
    DEBUG_PORT.print(getColorNameByStaffValue(current));
  }else{
    DEBUG_PORT.print("unknown");
  }

  DEBUG_PORT.print(", ");
}

void updateColors(SensorNode* node, size_t idx, void* args){
  if(idx < (sizeof(register_map)/sizeof(uint8_t))){
    size_t group_len = (sizeof(column_map_t)/sizeof(uint8_t));
    uint8_t* p_base = (uint8_t*)(&register_map);
    size_t offset = idx*group_len;
    uint8_t* p_reg = p_base + offset;

    uint8_t r = *(p_reg + 1);
    uint8_t g = *(p_reg + 2);
    uint8_t b = *(p_reg + 3);

    //           G  R  B
    *node = CRGB(g, r, b);
  }
}

void updateSensors( void ){

  while(1){
    
    bool was_updated = full_update;
    sensors.forEachRandOrder(detectAndTransmit, &full_update);

    DEBUG_PORT.print("Column: [ ");
    sensors.forEach(printInfo, &full_update);
    DEBUG_PORT.println("]");
    
    if(was_updated){
      full_update = false;
    }
  }
}
