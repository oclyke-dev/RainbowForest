
void networkRequestEvent( void ){
  uint8_t* p_reg = (uint8_t*)(&register_map);
  size_t written = NetworkWire.write((p_reg + reg_address), (sizeof(register_map)/sizeof(uint8_t)) - reg_address);
  // this sends the entire remaining memory map
}

// when data is received from the controller
void networkReceiveEvent(int bytes_received)
{
  if(bytes_received == 0){ return; }

  int bytes_data = bytes_received - 1;
  size_t node_data_size = (sizeof(column_map_t)/sizeof(uint8_t));
  uint8_t sub_index = 0x00;
  uint8_t data = 0x00;

  uint8_t* p_reg = (uint8_t*)(&register_map);
  uint8_t* p_pro = (uint8_t*)(&protection_map);

  reg_address = NetworkWire.read(); // get the beginning offset
  
  for(int idx = 0; idx < bytes_data; idx++){
    data = NetworkWire.read();
    if(reg_address < (sizeof(register_map)/sizeof(uint8_t))){
      sub_index = reg_address % node_data_size;
      *(p_reg + reg_address) &= ~(*(p_pro + sub_index));          // clear writeable bits for incoming value
      *(p_reg + reg_address) |= (data & (*(p_pro + sub_index)));  // or in valid bits from the data
    }

    reg_address += 1;
  }

  if(bytes_data != 0){
    sensors.forEachRandOrder(updateColors, NULL);
  }
}

void NETWORK_WIRE_IT_HANDLER(void) {
  NetworkWire.onService();
}
