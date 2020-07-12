/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioPlaySdWav           playSdWav4;     //xy=208,777
AudioPlaySdWav           playSdWav1;     //xy=209,641
AudioPlaySdWav           playSdWav2;     //xy=209,685
AudioPlaySdWav           playSdWav3;     //xy=209,731
AudioPlaySdWav           playSdWav7;     //xy=210,934
AudioPlaySdWav           playSdWav5;     //xy=211,826
AudioPlaySdWav           playSdWav6;     //xy=211,875
AudioMixer4              mixer4; //xy=426,911
AudioMixer4              mixer3; //xy=427,839
AudioMixer4              mixer1;         //xy=430,645
AudioMixer4              mixer2; //xy=430,714
AudioMixer4              mixer5;  //xy=651,671
AudioMixer4              mixer6; //xy=657,765
AudioOutputI2S           i2s1;           //xy=861,682
AudioConnection          patchCord1(playSdWav4, 0, mixer1, 3);
AudioConnection          patchCord2(playSdWav4, 1, mixer2, 3);
AudioConnection          patchCord3(playSdWav1, 0, mixer1, 0);
AudioConnection          patchCord4(playSdWav1, 1, mixer2, 0);
AudioConnection          patchCord5(playSdWav2, 0, mixer1, 1);
AudioConnection          patchCord6(playSdWav2, 1, mixer2, 1);
AudioConnection          patchCord7(playSdWav3, 0, mixer1, 2);
AudioConnection          patchCord8(playSdWav3, 1, mixer2, 2);
AudioConnection          patchCord9(playSdWav7, 0, mixer3, 2);
AudioConnection          patchCord10(playSdWav7, 1, mixer4, 2);
AudioConnection          patchCord11(playSdWav5, 0, mixer3, 0);
AudioConnection          patchCord12(playSdWav5, 1, mixer4, 0);
AudioConnection          patchCord13(playSdWav6, 0, mixer3, 1);
AudioConnection          patchCord14(playSdWav6, 1, mixer4, 1);
AudioConnection          patchCord15(mixer4, 0, mixer6, 1);
AudioConnection          patchCord16(mixer3, 0, mixer5, 1);
AudioConnection          patchCord17(mixer1, 0, mixer5, 0);
AudioConnection          patchCord18(mixer2, 0, mixer6, 0);
AudioConnection          patchCord19(mixer5, 0, i2s1, 0);
AudioConnection          patchCord20(mixer6, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=855,621
// GUItool: end automatically generated code


// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14


#include "src/components/staff/staff.h"
#include "src/components/uart_bridge/uart_bridge.h"
#include "src/components/cart/cart.h"

#include "src/components/configuration/configuration.h"

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)
#define DEBUG_VERBOSE (true)

#define BRIDGE_PORT Serial1
#define BRIDGE_BAUD (115200)
UARTBridge <cart_t> cartBridge(BRIDGE_PORT);
cart_t cart;

IntervalTimer playbackTimer;
bool playbackRunning = true;

typedef uint8_t staff_data_t;
Staff <staff_data_t> staff;

String instrument, note, sound;
String fileType = ".WAV";
char fileName[13];


void onCartReception(cart_t* cart, void* args){
  staff[cart->col][cart->row] = cart->val;
//  staff.showStaffDebug();
}


void playColumn( void ){
  //make player in here
  random_cart();
  
  static volatile uint32_t playbackColumn = 0;
  staff[playbackColumn][cart.row] = cart.val;
  if(playbackRunning){
    staff_data_t* data = staff[playbackColumn];
  
    for(uint32_t indi = 0; indi < STAFF_ROWS; indi++){
      
      //setting the note of the instrument, based on the position in the column array
      if(indi == 0){
        note = "C";
      }
      else if(indi == 1){
        note = "D";
      }
      else if(indi == 2){
        note = "E";
      }
      else if(indi == 3){
        note = "F";
      }
      else if(indi == 4){
        note = "G";
      }
      else if(indi == 5){
        note = "A";
      }
      else if(indi == 6){
        note = "B";
      }
      
      //setting the instrument type based on the value in the specified array index
      if(*(data + indi) == 0){
        instrument = "NONE";
      }
      else if(*(data + indi) == 1){
        instrument = "TRUMPET";
      }
      else if(*(data + indi) == 2){
        instrument = "FLUTE";
      }
      else if(*(data + indi) == 3){
        instrument = "GUITAR";
      }
      else if(*(data + indi) == 4){
        instrument = "DRUM";
      }
      else if(*(data + indi) == 5){
        instrument = "PIANO";
      }
      
      //creating the file name as a string
      sound = instrument + note + fileType;
      
      //turning string into a char array to pass into the teensy play function
      sound.toCharArray(fileName, 13);

      //sending the file name to the appropriate .WAV player
      if(indi == 0){
        if (playSdWav1.isPlaying() == false) {
          Serial.print("Start playing ");
          Serial.println(fileName);
          playSdWav1.play("PIANOC.WAV");
          delay(20); // wait for library to parse WAV info
        }
      }
      else if(indi == 1){
        if (playSdWav2.isPlaying() == false) {
          Serial.print("Start playing ");
          Serial.println(fileName);
          playSdWav2.play(fileName);
          delay(20); // wait for library to parse WAV info
        }
      }
      else if(indi == 2){
        if (playSdWav3.isPlaying() == false) {
          //Serial.println("Start playing 1");
          playSdWav3.play(fileName);
          delay(20); // wait for library to parse WAV info
        }
      }
      else if(indi == 3){
        if (playSdWav4.isPlaying() == false) {
          //Serial.println("Start playing 1");
          playSdWav4.play(fileName);
          delay(20); // wait for library to parse WAV info
        }
      }
      else if(indi == 4){
        if (playSdWav5.isPlaying() == false) {
          //Serial.println("Start playing 1");
          playSdWav5.play(fileName);
          delay(20); // wait for library to parse WAV info
        }
      }
      else if(indi == 5){
        if (playSdWav6.isPlaying() == false) {
          //Serial.println("Start playing 1");
          playSdWav6.play(fileName);
          delay(20); // wait for library to parse WAV info
        }
      }
      else if(indi == 6){
        if(playSdWav1.isPlaying()){
          playSdWav1.stop();
        }
        if (playSdWav7.isPlaying() == false) {
          //Serial.println("Start playing 1");
          playSdWav7.play(fileName);
          delay(20); // wait for library to parse WAV info
        }
      }
      
    //  DEBUG_PORT.print(*(data + indi));
      
    //  DEBUG_PORT.print(" ");
    //  DEBUG_PORT.print(fileName);
    }
    delay(1000);
    DEBUG_PORT.println();
    DEBUG_PORT.print(playbackColumn);    
    DEBUG_PORT.println();
    
    
    playbackColumn++;
    if(playbackColumn >= STAFF_COLS){
      playbackColumn = 0;
    }
  }
}

void random_cart( void ){
  cart.col = random(0, STAFF_COLS);
  cart.row = random(0, STAFF_ROWS);
  cart.val = random(0, STAFF_VALS);
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
  if(DEBUG_VERBOSE){ while(!DEBUG_PORT){}; }

  BRIDGE_PORT.begin(BRIDGE_BAUD);
  cartBridge.onReceive(onCartReception, NULL);

  staff.setDebugVerbose(DEBUG_VERBOSE);
  staff.setDebugStream(DEBUG_PORT);
  staff.setSize(STAFF_COLS, STAFF_ROWS);

  staff.setDebugVerbose(false);
  DEBUG_PORT.print("Staff Bit Depth: ");
  DEBUG_PORT.println(staff.getBitDepth());
  staff.setDebugVerbose(DEBUG_VERBOSE);

  playbackTimer.begin(playColumn, PLAYBACK_PERIOD_DEFAULT);

  Serial.begin(9600);
  AudioMemory(40);
  sgtl5000_1.enable();
  sgtl5000_1.volume(.5);
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  pinMode(13, OUTPUT); // LED on pin 13
  /***mixer1.gain(0, .2);
  mixer1.gain(1, .2);
  mixer1.gain(2, .2);
  mixer1.gain(3, .2);
  mixer2.gain(0, .2);
  mixer2.gain(1, .2);
  mixer2.gain(2, .2);
  mixer2.gain(3, .2);
  mixer5.gain(0, .2);
  mixer5.gain(1, .2);***/
  delay(1000);

  
}

void loop() {

  cartBridge.check();
  if (playSdWav1.isPlaying() == false) {
          Serial.println("LOOP");
          playSdWav1.play("PIANOC.WAV");
          delay(20); // wait for library to parse WAV info
        }
}
