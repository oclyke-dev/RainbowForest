/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

void detectAndTransmit(SensorNode* node, size_t idx, void* args){
  bool force_update = *((bool*)args);

  node->read();
  
  rgb_f_t rgb;
  rgb.r = rgbElemByUI16(node->getRed());
  rgb.g = rgbElemByUI16(node->getGreen());
  rgb.b = rgbElemByUI16(node->getBlue());

  DEBUG_PORT.print(idx);
  DEBUG_PORT.print(": ");
  
  size_t val;
  if(COLOR_DETECT_OK == detectedColor(&rgb, &val)){
    if((val != staff[0][idx]) || (force_update)){
      staff[0][idx] = (staff_data_t)val;
      sendRV(idx, val);
      DEBUG_PORT.print("(X) ");
      if(force_update){
        DEBUG_PORT.print("(F) ");
      }
    }
    DEBUG_PORT.print(detectable_colors[val].name);
  }else{
    DEBUG_PORT.print("unknown");
  }

  DEBUG_PORT.print(", ");

  delay(esp_random() & (uint32_t)0x0000003F);
}

void updateSensors( void* args ){

  FastLED.addLeds<WS2811, DATA_PIN, RGB>(sensors.getControl(), sensors.getNumControlElements()); // have to add leds in main sketch before sensors.begin()
  SensorStatus_e retval = sensors.begin();
  DEBUG_PORT.print("sensors.begin() returned: "); DEBUG_PORT.println(retval);

  while(1){
    DEBUG_PORT.print("Column: [");
    bool was_updated = full_update;
    sensors.forEachRandOrder(detectAndTransmit, &full_update);
    if(was_updated){
      full_update = false;
    }
    DEBUG_PORT.println("]");
  }
}
