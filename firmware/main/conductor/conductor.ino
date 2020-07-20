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


#define PLAY_BUTTON_PIN 8

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

volatile bool playNext = false;
uint8_t currentColumn = 0;

#define PLAYBACK_BPM_MAX (180) //BPM of 180 333333
#define PLAYBACK_BPM_MIN (60) //BPM of 60 1000000
uint8_t bpm;//Should be mapped between 60 and 180

String instrument, note, sound;
String fileType = ".WAV";
char fileName[13];


void onCartReception(cart_t* cart, void* args){
  staff[cart->col][cart->row] = cart->val;

}

static volatile uint32_t playbackColumn = 0;

/***
void playbuttonPress(){
  static uint32_t debounce = 0;
  uint32_t currentTime = millis();  
  if(currentTime >= debounce){
    debounce = currentTime+500;
    DEBUG_PORT.println("Pressed");
    //playNext = true;
  }
}
***/

void playbackISR( void ){

  random_cart();
  
  staff[playbackColumn][cart.row] = cart.val;
  if(playbackRunning){
    staff_data_t* data = staff[playbackColumn];
    
    for(uint32_t indi = 0; indi < STAFF_ROWS; indi++){   
      
      DEBUG_PORT.print(*(data + indi));
      DEBUG_PORT.print(" ");
    }

   
    DEBUG_PORT.println();
    DEBUG_PORT.print(playbackColumn);
    DEBUG_PORT.print(" ");
    DEBUG_PORT.print(AudioProcessorUsage());  
    DEBUG_PORT.println();
    
    
    playbackColumn++;
    if(playbackColumn >= STAFF_COLS){
      playbackColumn = 0;
    }
  

  playNext = true;

  }
  
}

void random_cart( void ){
  cart.col = random(0, STAFF_COLS);
  cart.row = random(0, STAFF_ROWS);
  cart.val = random(0, 6);   
}

/***
int BPMconverter(uint8_t bpm){
  float temp = bpm;
  return 1000000/(temp/60);
}
***/

void playColumn(int column){


    DEBUG_PORT.println("Playing");
    //stops all players that may currently be playing, makes it so sounds longer than the playback time don't interfere
    playSdWav1.stop();
    playSdWav2.stop();
    playSdWav3.stop();
    playSdWav4.stop();
    playSdWav5.stop();
    playSdWav6.stop();
    playSdWav7.stop();

    staff_data_t* data = staff[column];
    
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
        instrument = "GUITAR";
      }
      else if(*(data + indi) == 2){
        instrument = "TRUMPET";
      }
      else if(*(data + indi) == 3){
        instrument = "FLUTE";
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

          playSdWav1.play(fileName);
          DEBUG_PORT.print("Playing: ");
          DEBUG_PORT.println(fileName);
        }
      }
      
      else if(indi == 1){
        if (playSdWav2.isPlaying() == false) {
          
          playSdWav2.play(fileName);
          DEBUG_PORT.print("Playing: ");
          DEBUG_PORT.println(fileName);
        }
      }
      else if(indi == 2){
        if (playSdWav3.isPlaying() == false) {
          
          playSdWav3.play(fileName);
          DEBUG_PORT.print("Playing: ");
          DEBUG_PORT.println(fileName);
        }
      }
      else if(indi == 3){
        if (playSdWav4.isPlaying() == false) {
          
          playSdWav4.play(fileName);
          DEBUG_PORT.print("Playing: ");
          DEBUG_PORT.println(fileName);
        }
      }
      else if(indi == 4){
        if (playSdWav5.isPlaying() == false) {
          
          playSdWav5.play(fileName);
          DEBUG_PORT.print("Playing: ");
          DEBUG_PORT.println(fileName);
        }
      }
      else if(indi == 5){
        if (playSdWav6.isPlaying() == false) {
          
          playSdWav6.play(fileName);
          DEBUG_PORT.print("Playing: ");
          DEBUG_PORT.println(fileName);
        }
      }
      else if(indi == 6){
        if(playSdWav1.isPlaying()){
          playSdWav1.stop();
        }
        if (playSdWav7.isPlaying() == false) {
          
          playSdWav7.play(fileName);
          DEBUG_PORT.print("Playing: ");
          DEBUG_PORT.println(fileName);
        }
      }

    }
    DEBUG_PORT.print("Current Column: ");
    DEBUG_PORT.println(currentColumn);

    if(currentColumn == 15){
      currentColumn = 0;
    }  
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

  playbackTimer.begin(playbackISR, 600000);
  
  AudioMemory(40);
  sgtl5000_1.enable();
  sgtl5000_1.volume(1);
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  
  mixer1.gain(0, .2);
  mixer1.gain(1, .2);
  mixer1.gain(2, .2);
  mixer1.gain(3, .2);
  
  mixer2.gain(0, .2);
  mixer2.gain(1, .2);
  mixer2.gain(2, .2);
  mixer2.gain(3, .2);
  
  mixer3.gain(0, .2);
  mixer3.gain(1, .2);
  mixer3.gain(2, .2);

  mixer4.gain(0, .2);
  mixer4.gain(1, .2);
  mixer4.gain(2, .2);
 
  mixer5.gain(0, .2);
  mixer5.gain(1, .2);
  
  mixer6.gain(0, .2);
  mixer6.gain(1, .2);

  pinMode(PLAY_BUTTON_PIN, INPUT_PULLUP);

  //attachInterrupt(digitalPinToInterrupt(PLAY_BUTTON_PIN), playbuttonPress, FALLING);
}

void loop() {
  cartBridge.check();

  
 //playbackTimer.update(bpm);//playbackTimer.update() sets a new interval 
  
  if(playNext){
    playNext = false;
    playColumn(currentColumn++);
   // if(currentColumn == 15){
   //   playNext = false;
   // }
  }
}
