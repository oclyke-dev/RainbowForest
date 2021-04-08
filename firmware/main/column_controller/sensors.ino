/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

void read_idler( void ){
  FastLED.show(); // allow led updates while reading
}

void detect(SensorNode* node, size_t idx, void* args){
  node->read(read_idler);
  
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
  
  DEBUG_PORT.printf("%d: ", idx);

  staff_data_t current = staff[0][idx];
  if(detection_ok_staff[0][idx]){
    DEBUG_PORT.printf("%s", getColorNameByStaffValue(current));
  }else{
    DEBUG_PORT.printf("unknown");
  }

  DEBUG_PORT.printf(", ");
}
