/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define NOTE_PLAYERS (STAFF_ROWS + 1) // demoinstrating up to 8 notes
#define PLAYER_MIXERS (2)
AudioPlaySdWav  notePlayersAlpha[NOTE_PLAYERS];
AudioPlaySdWav  notePlayersBeta[NOTE_PLAYERS];
AudioMixer4     playerMixersAlphaLeft[PLAYER_MIXERS];
AudioMixer4     playerMixersAlphaRight[PLAYER_MIXERS];
AudioMixer4     playerMixersBetaLeft[PLAYER_MIXERS];
AudioMixer4     playerMixersBetaRight[PLAYER_MIXERS];
AudioMixer4     postMixerLeft;
AudioMixer4     postMixerRight;
AudioOutputI2S           i2s1;
AudioControlSGTL5000     sgtl5000_1;

// left side of alpha note players
AudioConnection stage1CordAlphaLeft0(notePlayersAlpha[0], 0, playerMixersAlphaLeft[0], 0);
AudioConnection stage1CordAlphaLeft1(notePlayersAlpha[1], 0, playerMixersAlphaLeft[0], 1);
AudioConnection stage1CordAlphaLeft2(notePlayersAlpha[2], 0, playerMixersAlphaLeft[0], 2);
AudioConnection stage1CordAlphaLeft3(notePlayersAlpha[3], 0, playerMixersAlphaLeft[0], 3);
AudioConnection stage1CordAlphaLeft4(notePlayersAlpha[4], 0, playerMixersAlphaLeft[1], 0);
AudioConnection stage1CordAlphaLeft5(notePlayersAlpha[5], 0, playerMixersAlphaLeft[1], 1);
AudioConnection stage1CordAlphaLeft6(notePlayersAlpha[6], 0, playerMixersAlphaLeft[1], 2);
AudioConnection stage1CordAlphaLeft7(notePlayersAlpha[7], 0, playerMixersAlphaLeft[1], 3);

// right side of alpha note players
AudioConnection stage1CordAlphaRight0(notePlayersAlpha[0], 1, playerMixersAlphaRight[0], 0);
AudioConnection stage1CordAlphaRight1(notePlayersAlpha[1], 1, playerMixersAlphaRight[0], 1);
AudioConnection stage1CordAlphaRight2(notePlayersAlpha[2], 1, playerMixersAlphaRight[0], 2);
AudioConnection stage1CordAlphaRight3(notePlayersAlpha[3], 1, playerMixersAlphaRight[0], 3);
AudioConnection stage1CordAlphaRight4(notePlayersAlpha[4], 1, playerMixersAlphaRight[1], 0);
AudioConnection stage1CordAlphaRight5(notePlayersAlpha[5], 1, playerMixersAlphaRight[1], 1);
AudioConnection stage1CordAlphaRight6(notePlayersAlpha[6], 1, playerMixersAlphaRight[1], 2);
AudioConnection stage1CordAlphaRight7(notePlayersAlpha[7], 1, playerMixersAlphaRight[1], 3);

// left side of beta note players
AudioConnection stage1CordBetaLeft0(notePlayersBeta[0], 0, playerMixersBetaLeft[0], 0);
AudioConnection stage1CordBetaLeft1(notePlayersBeta[1], 0, playerMixersBetaLeft[0], 1);
AudioConnection stage1CordBetaLeft2(notePlayersBeta[2], 0, playerMixersBetaLeft[0], 2);
AudioConnection stage1CordBetaLeft3(notePlayersBeta[3], 0, playerMixersBetaLeft[0], 3);
AudioConnection stage1CordBetaLeft4(notePlayersBeta[4], 0, playerMixersBetaLeft[1], 0);
AudioConnection stage1CordBetaLeft5(notePlayersBeta[5], 0, playerMixersBetaLeft[1], 1);
AudioConnection stage1CordBetaLeft6(notePlayersBeta[6], 0, playerMixersBetaLeft[1], 2);
AudioConnection stage1CordBetaLeft7(notePlayersBeta[7], 0, playerMixersBetaLeft[1], 3);

// right side of beta note players
AudioConnection stage1CordBetaRight0(notePlayersBeta[0], 1, playerMixersBetaRight[0], 0);
AudioConnection stage1CordBetaRight1(notePlayersBeta[1], 1, playerMixersBetaRight[0], 1);
AudioConnection stage1CordBetaRight2(notePlayersBeta[2], 1, playerMixersBetaRight[0], 2);
AudioConnection stage1CordBetaRight3(notePlayersBeta[3], 1, playerMixersBetaRight[0], 3);
AudioConnection stage1CordBetaRight4(notePlayersBeta[4], 1, playerMixersBetaRight[1], 0);
AudioConnection stage1CordBetaRight5(notePlayersBeta[5], 1, playerMixersBetaRight[1], 1);
AudioConnection stage1CordBetaRight6(notePlayersBeta[6], 1, playerMixersBetaRight[1], 2);
AudioConnection stage1CordBetaRight7(notePlayersBeta[7], 1, playerMixersBetaRight[1], 3);

// left side mixers
AudioConnection stage2CordLeft0(playerMixersAlphaLeft[0], 0, postMixerLeft, 0);
AudioConnection stage2CordLeft1(playerMixersAlphaLeft[1], 0, postMixerLeft, 1);
AudioConnection stage2CordLeft2(playerMixersBetaLeft[0], 0, postMixerLeft, 2);
AudioConnection stage2CordLeft3(playerMixersBetaLeft[1], 0, postMixerLeft, 3);

// right side mixers
AudioConnection stage2CordRight0(playerMixersAlphaRight[0], 0, postMixerRight, 0);
AudioConnection stage2CordRight1(playerMixersAlphaRight[1], 0, postMixerRight, 1);
AudioConnection stage2CordRight2(playerMixersBetaRight[0], 0, postMixerRight, 2);
AudioConnection stage2CordRight3(playerMixersBetaRight[1], 0, postMixerRight, 3);

// output
AudioConnection stage3CordLeft(postMixerLeft, 0, i2s1, 0);
AudioConnection stage3CordRight(postMixerRight, 0, i2s1, 1);

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

#define WATCHDOG_AUDIO_PROC_USAGE_HISTORY_LEN (2*STAFF_COLS)

#define NUM_SOUNDS (STAFF_VALS * STAFF_ROWS)
const char* sounds[NUM_SOUNDS] = {
  "gC.wav", "gD.wav", "gE.wav", "gF.wav", "gG.wav", "gA.wav", "gB.wav",
  "tC.wav", "tD.wav", "tE.wav", "tF.wav", "tG.wav", "tA.wav", "tB.wav",
  "fC.wav", "fD.wav", "fE.wav", "fF.wav", "fG.wav", "fA.wav", "fB.wav",
  "dC.wav", "dD.wav", "dE.wav", "dF.wav", "dG.wav", "dA.wav", "dB.wav",
  "gC.wav", "gD.wav", "gE.wav", "gF.wav", "gG.wav", "gA.wav", "gB.wav",
};
#define SOUND(I, N) sounds[(STAFF_ROWS * (I - INSTRUMENT_GUITAR)) + N]

void audio_setup( void ){
  AudioMemory(64);
  
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.20);
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    Serial.println("Unable to access the SD card");
    delay(1000);
    sw_reset();
  }
}

uint32_t FreeMem(){ // for Teensy 3.0
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t) &stackTop;

    // current position of heap.
    void* hTop = malloc(1);
    heapTop = (uint32_t) hTop;
    free(hTop);

    // The difference is (approximately) the free, available ram.
    return stackTop - heapTop;
}

float avgProcUsage = 0.0f;
uint32_t audio_watchdog( void ){
  static float procUsageHistory[WATCHDOG_AUDIO_PROC_USAGE_HISTORY_LEN];
  float audioProcNow = AudioProcessorUsage();
  float accumulator = 0.0f;
  
  bool consecutive = true;
  uint32_t consecutive_identical = 0;

  for(size_t idx = WATCHDOG_AUDIO_PROC_USAGE_HISTORY_LEN - 1; idx > 0; idx--){
    procUsageHistory[idx] = procUsageHistory[idx - 1];
  }
  procUsageHistory[0] = audioProcNow;

  for(size_t idx = 0; idx < WATCHDOG_AUDIO_PROC_USAGE_HISTORY_LEN; idx++){
    float historicalUsage = procUsageHistory[idx];
    accumulator += historicalUsage;
    if(consecutive && (abs(audioProcNow - historicalUsage) < 1e-4)){
      consecutive_identical++;
    }else{
      consecutive = false;
    }
  }
  avgProcUsage = accumulator / WATCHDOG_AUDIO_PROC_USAGE_HISTORY_LEN;

  if(avgProcUsage >= 10.0f){
    if(consecutive_identical >= 10){
      sw_reset();
    }
  }

  if(consecutive_identical >= WATCHDOG_AUDIO_PROC_USAGE_HISTORY_LEN){
    sw_reset();
  }

  return consecutive_identical;
}

void showPlayHead( uint8_t col ){
  uint8_t prev_col = col - 1;
  if(col == 0){
    prev_col = 15;
  }
  CHSV hsv((((float)col)/((float)STAFF_COLS)) * 255, 255, 255);
  CRGB rgb(0, 0, 0);
  for(size_t note = 0; note < STAFF_ROWS; note++){
//    CHSV hsv((((float)note)/((float)STAFF_ROWS)) * 255, 255, 255);
//    CRGB rgb(0, 0, 0);
    setSensorHSV(hsv, col, note);
    setSensorRGB(rgb, prev_col, note);
  }
}

void playColumn( uint8_t col ){  
  static uint8_t prev_players = 0;
  static uint8_t curr_players = 0;

  uint32_t consecutive_identical_cpu_usage = audio_watchdog(); // try to detect and recover from cold snaps

  showPlayHead(col);

  AudioPlaySdWav* players = notePlayersAlpha;
  prev_players = curr_players;
  if(curr_players == 0x01){
    players = notePlayersBeta;
    curr_players = 0x00;
  }else if(curr_players == 0x00){
    curr_players = 0x01;
  }
  
  
  DEBUG_PORT.print("Playing column: ");
  if(col < 10){
    DEBUG_PORT.print(0);
  }
  DEBUG_PORT.print(col);
  DEBUG_PORT.print(", [");

  for(size_t note = 0; note < STAFF_ROWS; note++){
    staff_data_t instrument = staff[col][note];
    const char* filename = " NONE ";
    
    players[note].stop();
    if(instrument != STAFF_VAL_NONE){
      filename = SOUND(instrument, note);
      players[note].play(filename);
      delay(5); // A brief delay for the library read WAV info
    }
    DEBUG_PORT.print(note);
    DEBUG_PORT.print(" (");
    DEBUG_PORT.print(filename);
    DEBUG_PORT.print(")");
    DEBUG_PORT.print(", ");
  }
  DEBUG_PORT.print("] ");
  DEBUG_PORT.print("by: ");
  DEBUG_PORT.print((prev_players) ? "beta " : "alpha");
  

  DEBUG_PORT.print(" {CPU use: ");
  float audioProcNow = AudioProcessorUsage();
  static float prevAudioProc = 0;
  if(audioProcNow < 100.0){
    DEBUG_PORT.print(" ");
  }
  if(audioProcNow < 10){
    DEBUG_PORT.print(" ");
  }
  DEBUG_PORT.print(audioProcNow, 5);
  DEBUG_PORT.print("/");
  float audioProcMax = AudioProcessorUsageMax();
  if(audioProcMax < 100.0){
    DEBUG_PORT.print(" ");
  }
  if(audioProcMax < 10){
    DEBUG_PORT.print(" ");
  }
  DEBUG_PORT.print(audioProcMax);
  DEBUG_PORT.print(" (");
  DEBUG_PORT.print(avgProcUsage);
  DEBUG_PORT.print(")");
  DEBUG_PORT.print("    ");
  DEBUG_PORT.print("Memory: ");
  DEBUG_PORT.print(AudioMemoryUsage());
  DEBUG_PORT.print("/");
  DEBUG_PORT.print(AudioMemoryUsageMax());
  DEBUG_PORT.print(", ci: ");
  DEBUG_PORT.print(consecutive_identical_cpu_usage);
  DEBUG_PORT.print("}");
  DEBUG_PORT.println();

  prevAudioProc = audioProcNow;
}
