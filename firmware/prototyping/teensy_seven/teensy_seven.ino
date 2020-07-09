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


// Use these with the audio adaptor board
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14


IntervalTimer playTimer;
//#define PLAY_INTERVAL 250000
#define PLAY_INTERVAL 2500000
volatile bool playing = true;
volatile uint32_t next_column = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(8);

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  playTimer.begin(playNotes, PLAY_INTERVAL);
}

void loop() {
  // put your main code here, to run repeatedly:


}


// functions called by IntervalTimer should be short, run as quickly as
// possible, and should avoid calling other functions if possible.
void playNotes() {
  if(playing){
    Serial.println("note");
    
//    next_column++;
//    if(next_column >= STAFF_COLS){
//      next_column = 0;
//    }


    Serial.print("CPU (all): ");
    Serial.print(AudioProcessorUsage());
    Serial.print(",");
    Serial.print(AudioProcessorUsageMax());
    Serial.print("    ");
    Serial.print("Memory (all): ");
    Serial.print(AudioMemoryUsage());
    Serial.print(",");
    Serial.print(AudioMemoryUsageMax());
    Serial.println("    ");



    // Start playing the file.  This sketch continues to
    // run while the file plays.
    playSdWav1.play("TC45.wav");
  
    // A brief delay for the library read WAV info
    delay(5);

    
  }
}
