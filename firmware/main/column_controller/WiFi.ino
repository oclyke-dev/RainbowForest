void setColorFromCat(SensorNode* node, size_t idx, void* args){
  uint8_t r = ((cat.rH << 4) | (cat.rL & 0x0F));
  uint8_t g = ((cat.gH << 4) | (cat.gL & 0x0F));
  uint8_t b = ((cat.bH << 4) | (cat.bL & 0x0F));
  
  *(node) = CRGB(g, r, b);
}

void handleCat( void ){
  uint8_t r = ((cat.rH << 4) | (cat.rL & 0x0F));
  uint8_t g = ((cat.gH << 4) | (cat.gL & 0x0F));
  uint8_t b = ((cat.bH << 4) | (cat.bL & 0x0F));

  DEBUG_PORT.println();
  DEBUG_PORT.print("Handling CAT. { col: ");
  DEBUG_PORT.print(cat.col);
  DEBUG_PORT.print(", row: ");
  DEBUG_PORT.print(cat.row);
  DEBUG_PORT.print(" } ");
  
  if(cat.row == COMMAND_REQ_FULL_UPDATE){
    DEBUG_PORT.println(" Full Update Requested");
    
    full_update = true;
    return;
  }
  
  if(cat.row == COMMAND_SET_COLUMN_COLOR){
    DEBUG_PORT.print(" Column Color Set: {");
    DEBUG_PORT.print(" r: ");
    DEBUG_PORT.print(r);
    DEBUG_PORT.print(", g: ");
    DEBUG_PORT.print(g);
    DEBUG_PORT.print(", b: ");
    DEBUG_PORT.print(b);
    DEBUG_PORT.println();
    
    sensors.forEach(setColorFromCat, NULL);
    return;
  }

  DEBUG_PORT.print(" individual color set: {");
  if((cat.col < STAFF_COLS) && (cat.row < STAFF_ROWS)){
    DEBUG_PORT.print(" r: ");
    DEBUG_PORT.print(r);
    DEBUG_PORT.print(", g: ");
    DEBUG_PORT.print(g);
    DEBUG_PORT.print(", b: ");
    DEBUG_PORT.print(b);
    DEBUG_PORT.println();

    *(sensors[cat.row]) = CRGB(g, r, b);
    return;
  }

  DEBUG_PORT.println(" unrecognized CAT");
}

void sendRV(uint8_t row, uint8_t val){
  cart.row = row;
  cart.val = val;

  String msg = 
  "{"
  "\"col\": " + String(CONTROLLER_COLUMN) + ","
  "\"row\": " + String(row) + ","
  "\"val\": " + String(val) + ""
  "}";

  DEBUG_PORT.println(msg);
  String hub_name(NETWORK_HUB_NAME);
  mesh.sendSingle(hub_name, msg);
}
