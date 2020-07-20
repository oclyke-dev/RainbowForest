/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#define SDA_PIN 23
#define SCL_PIN 22
#define DAT_PIN 18

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

typedef enum _check_s_e {
  S_CHECK_SCL_SHRT_GND = 0,
  S_CHECK_SCL_SHRT_PWR,
  S_CHECK_SDA_SHRT_GND,
  S_CHECK_SDA_SHRT_PWR,
  S_CHECK_DAT_SHRT_GND,
  S_CHECK_DAT_SHRT_PWR,
  S_CHECK_SCL_SHRT_SDA,
  S_CHECK_SDA_SHRT_SCL,
  S_CHECK_SCL_SHRT_DAT,
  S_CHECK_DAT_SHRT_SCL,
  S_CHECK_SDA_SHRT_DAT,
  S_CHECK_DAT_SHRT_SDA,
} check_s_e;

#define CheckCodeEntry(e) e = (1 << S_##e)

typedef enum _check_codes_e {
  CheckCodeEntry(CHECK_SCL_SHRT_GND),
  CheckCodeEntry(CHECK_SCL_SHRT_PWR),
  CheckCodeEntry(CHECK_SDA_SHRT_GND),
  CheckCodeEntry(CHECK_SDA_SHRT_PWR),
  CheckCodeEntry(CHECK_DAT_SHRT_GND),
  CheckCodeEntry(CHECK_DAT_SHRT_PWR),
  CheckCodeEntry(CHECK_SCL_SHRT_SDA),
  CheckCodeEntry(CHECK_SDA_SHRT_SCL),
  CheckCodeEntry(CHECK_SCL_SHRT_DAT),
  CheckCodeEntry(CHECK_DAT_SHRT_SCL),
  CheckCodeEntry(CHECK_SDA_SHRT_DAT),
  CheckCodeEntry(CHECK_DAT_SHRT_SDA),

  // num checks
  CHECK_NUM_CHECKS
} check_codes_e;

bool shorted_to_gnd(uint8_t pin, uint32_t del = 0);
bool shorted_to_pwr(uint8_t pin, uint32_t del = 0);
bool shorted_to_pin(uint8_t pina, uint8_t pinb, uint32_t del = 0);

bool shorted_to_gnd(uint8_t pin, uint32_t del){
  pinMode(pin, INPUT_PULLUP);
  delay(del);
  bool result = (digitalRead(pin)) ? false : true;
  pinMode(pin, INPUT);
  return result;
}

bool shorted_to_pwr(uint8_t pin, uint32_t del){
  pinMode(pin, INPUT_PULLDOWN);
  delay(del);
  bool result = (digitalRead(pin)) ? true : false;
  pinMode(pin, INPUT);
  return result;
}

bool shorted_to_pin(uint8_t pina, uint8_t pinb, uint32_t del){
  pinMode(pina, INPUT_PULLUP);
  pinMode(pinb, OUTPUT);
  digitalWrite(pinb, LOW);
  delay(del);
  bool result = (digitalRead(pina)) ? false : true;
  pinMode(pina, INPUT);
  pinMode(pinb, INPUT);
  return result;
}

uint32_t cable_bad( void ){
  uint32_t errors = 0x0000;
  errors |= (shorted_to_gnd(SCL_PIN)) ? CHECK_SCL_SHRT_GND : 0x00;
  errors |= (shorted_to_pwr(SCL_PIN)) ? CHECK_SCL_SHRT_PWR : 0x00;
  errors |= (shorted_to_gnd(SDA_PIN)) ? CHECK_SDA_SHRT_GND : 0x00;
  errors |= (shorted_to_pwr(SDA_PIN)) ? CHECK_SDA_SHRT_PWR : 0x00;
  errors |= (shorted_to_gnd(DAT_PIN)) ? CHECK_DAT_SHRT_GND : 0x00;
  errors |= (shorted_to_pwr(DAT_PIN)) ? CHECK_DAT_SHRT_PWR : 0x00;
  errors |= (shorted_to_pin(SCL_PIN, SDA_PIN)) ? CHECK_SCL_SHRT_SDA : 0x00;
  errors |= (shorted_to_pin(SDA_PIN, SCL_PIN)) ? CHECK_SDA_SHRT_SCL : 0x00;
  errors |= (shorted_to_pin(SCL_PIN, DAT_PIN)) ? CHECK_SCL_SHRT_DAT : 0x00;
  errors |= (shorted_to_pin(DAT_PIN, SCL_PIN)) ? CHECK_DAT_SHRT_SCL : 0x00;
  errors |= (shorted_to_pin(SDA_PIN, DAT_PIN)) ? CHECK_SDA_SHRT_DAT : 0x00;
  errors |= (shorted_to_pin(DAT_PIN, SDA_PIN)) ? CHECK_DAT_SHRT_SDA : 0x00;
  return errors;
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
}

void loop() {
  delay(100);
  
  uint32_t errors = cable_bad();
  if(errors){
    DEBUG_PORT.print("errors: 0b");
    for(size_t idx = 0; idx < 32; idx++){
      if(errors < (((uint32_t)0x01) << (31 - idx))){
        DEBUG_PORT.print(0);
      }
    }
    DEBUG_PORT.print(errors, BIN);
    DEBUG_PORT.print(", time (ms): ");
    DEBUG_PORT.println(millis());
  }else{
    DEBUG_PORT.print("All Good! time (ms): ");
    DEBUG_PORT.println(millis());
  }
}
