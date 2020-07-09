#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

IntervalTimer playTimer;
#define PLAY_INTERVAL 250000
volatile bool playing = true;
volatile uint32_t next_column = 0;

AudioPlaySdWav           playWav1;
// Use one of these 3 output types: Digital I2S, Digital S/PDIF, or Analog DAC
AudioOutputI2S           audioOutput;
AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;

// Use these with the audio adaptor board
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

typedef struct _packet_t {
  uint8_t s = 0;
  uint8_t c = 0;
  uint8_t r = 0;
  uint8_t v = 0;
  uint8_t e = 0;
  uint8_t d = 0;
} packet_t;

packet_t packet = {0};

#define STAFF_ROWS 7
#define STAFF_COLS 16
#define STAFF_VALS 5
uint8_t staff[STAFF_ROWS*STAFF_COLS] = {0};

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  
  Serial.println("\nTeensy UART Control");

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

void loop()
{
  if(parse_packet(&Serial1, &packet)){
    Serial.println("Packet Received!");
    update_staff(&packet);
    print_staff(&Serial);
    
    if(!playWav1.isPlaying()){
      if(packet.v == 0){
        playFile("SDTEST1.WAV");
      }
      if(packet.v == 1){
        playFile("SDTEST2.WAV");
      }
      if(packet.v == 2){
        playFile("SDTEST3.WAV");
      }
      if(packet.v == 3){
        playFile("SDTEST4.WAV");
      }
    }
  }
}

bool parse_packet(Stream* stream, packet_t* packet){
  if(stream->available() < (sizeof(packet_t)/sizeof(uint8_t))){ return false; }
  uint8_t sum = 0;

  packet->s = stream->read();
  if(packet->s != 0x20){ return false; }

  packet->c = stream->read();
  packet->r = stream->read();
  packet->v = stream->read();
  packet->e = stream->read();
  packet->d = stream->read();

  sum += packet->s;
  sum += packet->c;
  sum += packet->r;
  sum += packet->v;
  sum += packet->e;

  if(packet->d != sum){ return false; }

  return true;
}

void playFile(const char *filename){
  Serial.print("Playing file: ");
  Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playWav1.play(filename);

  // A brief delay for the library read WAV info
  delay(5);
}

void update_staff(packet_t* packet){
  if(packet->c >= STAFF_COLS){ return; }
  if(packet->r >= STAFF_ROWS){ return; }
  if(packet->v >= STAFF_VALS){ return; }
  uint32_t index = (packet->c * STAFF_ROWS) + packet->r; // map the value to the right index of the array
  staff[index] = packet->v;
}

void print_staff( Stream* stream ){
  for(uint32_t row = 0; row < STAFF_ROWS; row++){
    for(uint32_t column = 0; column < STAFF_COLS; column++){
      uint32_t index = (column * STAFF_ROWS) + row;
      stream->print(staff[index]);
      if(column == next_column){
        stream->print(".");
      }
      stream->print(" ");
    }
    stream->println();
  }
  stream->println("\n");
}

// functions called by IntervalTimer should be short, run as quickly as
// possible, and should avoid calling other functions if possible.
void playNotes() {
  if(playing){
    Serial.println("note");
    next_column++;
    if(next_column >= STAFF_COLS){
      next_column = 0;
    }
    // play notes here
  }
  else{
    // not playing, start from beginning
    next_column = 0;
  }
}
