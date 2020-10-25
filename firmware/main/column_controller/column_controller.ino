/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// BOARD: SparkFun SAMD21 Mini Breakout

#include "src/components/cmt/cmt.h"
#include "src/components/staff/staff.h"
#include "src/components/sensor/sensor.h"
#include "src/components/configuration/configuration.h"

#include <Scheduler.h>
#include "FlashStorage.h"
#include <Wire.h>
#include "wiring_private.h" // pinPeripheral() function

#define DEBUG_PORT SerialUSB
#define DEBUG_BAUD (115200)

#define LED0 A0
#define LED1 A3
#define LED2 8
#define LED3 9

#define BUTTON0 10
#define BUTTON1 12

#define RAND_PIN A1

#define NETWORK_SDA_PIN 11
#define NETWORK_SCL_PIN 13
#define NETWORK_WIRE_IT_HANDLER SERCOM1_Handler
TwoWire NetworkWire(&sercom1, NETWORK_SDA_PIN, NETWORK_SCL_PIN);

#define COLUMN_SDA_PIN 4
#define COLUMN_SCL_PIN 3
TwoWire ColumnWire(&sercom0, COLUMN_SDA_PIN, COLUMN_SCL_PIN);

#define DATA_PIN 6
#define COLUMN_LEN (STAFF_ROWS)
SensorString sensors(COLUMN_LEN, DATA_PIN, &ColumnWire);

Staff <staff_data_t> staff;
Staff <staff_data_t> prev_staff;
Staff <bool> detection_ok_staff;

volatile bool button0 = false;
volatile bool button1 = false;

#define CONTROLLER_COLUMN (0) // indicates which column this controller reads (default if EEPROM unused)
FlashStorage(flash_column_number_plus_one, int);

uint8_t col = 0;
bool full_update = true;

// ram memory map for network i2c
volatile int reg_address = 0x00;
volatile column_map_t register_map[COLUMN_LEN]; // 

void setRowValue(uint8_t row, uint8_t value){
  size_t node_data_size = (sizeof(column_map_t)/sizeof(uint8_t));
  uint8_t* p_node = (uint8_t*)(&register_map[row]);
  uint8_t* p_val = p_node + 0;
  *(p_val) = value;
}

void showColumnNumber( uint8_t c ){
  digitalWrite(LED0, (c & (1 << 0)) ? HIGH : LOW);
  digitalWrite(LED1, (c & (1 << 1)) ? HIGH : LOW);
  digitalWrite(LED2, (c & (1 << 2)) ? HIGH : LOW);
  digitalWrite(LED3, (c & (1 << 3)) ? HIGH : LOW);
}

void setColumnNumber( uint8_t c ){
  if(c >= STAFF_COLS){ return; }
  col = c;
  flash_column_number_plus_one.write(col + 1);
  showColumnNumber(col);

  // initialize network i2c (when column number changes so does network address)
  NetworkWire.begin(NETWORK_ADDRESS_BY_COL(col));
  pinPeripheral(NETWORK_SDA_PIN, PIO_SERCOM);
  pinPeripheral(NETWORK_SCL_PIN, PIO_SERCOM);
  NetworkWire.onReceive(networkReceiveEvent);
  NetworkWire.onRequest(networkRequestEvent);

  DEBUG_PORT.print("Set column to ");
  DEBUG_PORT.println(col);
}

void button0ISR() {
  button0 = true;
}

void button1ISR() {
  button1 = true;
}

void handleUserInterrupts( void ){
  delay(25);  // leave ample time to update the sensors

  // button 0 changes the configured column number
  if(button0){
    button0 = false;
    static uint32_t debounce = 0;
    if(millis() > debounce){
      debounce = millis() + 50;
      col++;
      if(col >= STAFF_COLS){
        col = 0;
      }
      setColumnNumber(col);
    }
  }

  // button 1
  if(button1){
    button1 = false;
    DEBUG_PORT.println("button1 pressed");
  }
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
  // while(!DEBUG_PORT){}; // comment out for production!

  randomSeed(analogRead(RAND_PIN));

  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED0, LOW);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  pinMode(BUTTON0, INPUT_PULLUP);
  pinMode(BUTTON1, INPUT_PULLUP);
  attachInterrupt(BUTTON0, button0ISR, RISING);
  attachInterrupt(BUTTON1, button1ISR, RISING);

  // initialize the staff data containers
  staff.setDebugStream(DEBUG_PORT);
  staff.setSize(1, STAFF_ROWS);
  prev_staff.setSize(1, STAFF_ROWS);
  detection_ok_staff.setSize(1, STAFF_ROWS);

  // initialize the register map
  memset((void*)register_map, 0x00, sizeof(register_map));

  showColumnNumber(0xF); // show user that the board started up
  delay(250);

  // get the column number
  int column_number_plus_one = flash_column_number_plus_one.read();
  if(column_number_plus_one == 0){
    flash_column_number_plus_one.write(CONTROLLER_COLUMN + 1);
    setColumnNumber(flash_column_number_plus_one.read() - 1);
  }else{
    setColumnNumber(column_number_plus_one - 1);
  }

  DEBUG_PORT.print("Column Controller (");
  DEBUG_PORT.print(col);
  DEBUG_PORT.println(")");

  // initialize column i2c (when column number changes so does the i2c address)
  ColumnWire.begin();
  pinPeripheral(COLUMN_SDA_PIN, PIO_SERCOM);
  pinPeripheral(COLUMN_SCL_PIN, PIO_SERCOM);

  // initialize sensor string
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(sensors.getControl(), sensors.getNumControlElements()); // have to add leds in main sketch before sensors.begin()
  SensorStatus_e retval = sensors.begin();
  DEBUG_PORT.print("sensors.begin() returned: "); DEBUG_PORT.println(retval);

  // begin a loop to handle user input
  Scheduler.startLoop(handleUserInterrupts);

  button0 = false; // don't allow button presses until setup has completed
  button1 = false;
}

void loop() {
  updateSensors();
}