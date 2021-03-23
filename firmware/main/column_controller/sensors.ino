/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

void detect(SensorNode* node, size_t idx, void* args){
  node->read();
  
  rgb_f_t rgb;
  rgb.r = rgbElemByUI16(node->getRed());
  rgb.g = rgbElemByUI16(node->getGreen());
  rgb.b = rgbElemByUI16(node->getBlue());

  // for normal operation detect colors based on sensor results
  size_t color_index;
  if(COLOR_DETECT_OK == detectedColor(&rgb, &color_index)){
    staff_data_t val = detectable_colors[color_index].staff_val;
    detection_ok_staff[0][idx] = true;
    staff[0][idx] = (staff_data_t)val;
  }else{
    detection_ok_staff[0][idx] = false;
  }

  delay(random(64)); // additional delay to enhance blinking randomness
}

void detect_fake(SensorNode* node, size_t idx, void* args){
  staff[0][idx] = random(0, STAFF_VALS + 1);
  detection_ok_staff[0][idx] = true;

  delay(500);
  delay(random(64));
}

void printInfo(SensorNode* node, size_t idx, void* args){
  
  DEBUG_PORT.print(idx);
  DEBUG_PORT.print(": ");

  staff_data_t current = staff[0][idx];
  if(detection_ok_staff[0][idx]){
    DEBUG_PORT.print(getColorNameByStaffValue(current));
  }else{
    DEBUG_PORT.print("unknown");
  }

  DEBUG_PORT.print(", ");
}
