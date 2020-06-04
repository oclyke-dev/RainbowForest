
#include "Staff.h"

/////////////
// User Setup
#define DEBUG 1
#define SERIAL_BAUD_RATE 115200
#define SERIAL_PORT Serial
// End User Setup
/////////////////

// Staff info
#define RF_NUM_BARS (4)
Staff RFStave(RF_NUM_BARS); // declare a Staff object with some number of bars (in this case 4 for Rainbow Forest)


void setup() {
#ifdef DEBUG
  SERIAL_PORT.begin(SERIAL_BAUD_RATE);
  while(!SERIAL_PORT){};
  debug_print("Hello World\n");
#endif

  // Example song... (loosely based on Canon in D)
  
  RFStave.setNoteStatus(NOTE_STATUS_RED, 0, 0, 3); // note 0 is "A" - due to limited range we need to transpose up/down an octave at times
  RFStave.setNoteStatus(NOTE_STATUS_RED, 0, 1, 5);
  RFStave.setNoteStatus(NOTE_STATUS_RED, 0, 2, 0);
  RFStave.setNoteStatus(NOTE_STATUS_RED, 0, 3, 3);
  
  RFStave.setNoteStatus(NOTE_STATUS_RED, 1, 0, 2);
  RFStave.setNoteStatus(NOTE_STATUS_RED, 1, 1, 4);
  RFStave.setNoteStatus(NOTE_STATUS_RED, 1, 2, 0);
  RFStave.setNoteStatus(NOTE_STATUS_RED, 1, 3, 2);

  RFStave.setNoteStatus(NOTE_STATUS_RED, 2, 0, 1);
  RFStave.setNoteStatus(NOTE_STATUS_RED, 2, 1, 3);
  RFStave.setNoteStatus(NOTE_STATUS_RED, 2, 2, 5);
  RFStave.setNoteStatus(NOTE_STATUS_RED, 2, 3, 1);

  RFStave.setNoteStatus(NOTE_STATUS_RED, 3, 0, 0);
  RFStave.setNoteStatus(NOTE_STATUS_RED, 3, 1, 2);
  RFStave.setNoteStatus(NOTE_STATUS_RED, 3, 2, 5);
  RFStave.setNoteStatus(NOTE_STATUS_RED, 3, 3, 0);
  

  RFStave.printInfo( &SERIAL_PORT );
  SERIAL_PORT.print("Done printing staff info");
}

void loop() {

  RFStave.printInfo( &SERIAL_PORT );  // In the loop you could access the changing information in the staff structure. 
  
  // Here's another way to access particular note values:
  bar_t* p_bars = RFStave.getBarsPointer(); // a pointer to the internal bars structure

  note_status_t stat = p_bars[0].q_1.A;
  //           which bar ^          (this is an index into an array, so the range is [0, num_bars) )
  //       which quarter bar ^      (q_1, q_2, q_3, q_4 are all valid)
  //                  which note ^  (A, B, C, D, E, F, G -- all 7 notes in a scale)

  // The challenge is to write a program tthat, when playing is active, plays a combination of sounds at each pitch (A-G) with the correspoding instrument

  for(uint8_t ib = 0; ib < RFStave.getLengthBars(); ib++){
    for(uint8_t iq = 0; iq < 4; iq++){
      quarter_bar_t* p_quarter = (quarter_bar_t*)((quarter_bar_t*)&p_bars[ib] + iq); // get a pointer to the quarter

      // Print the column to the serial monitor for the time being... until we can make music play in its place
      Staff::printNoteStatus(p_quarter->G, &SERIAL_PORT); SERIAL_PORT.println();
      Staff::printNoteStatus(p_quarter->F, &SERIAL_PORT); SERIAL_PORT.println();
      Staff::printNoteStatus(p_quarter->E, &SERIAL_PORT); SERIAL_PORT.println();
      Staff::printNoteStatus(p_quarter->D, &SERIAL_PORT); SERIAL_PORT.println();
      Staff::printNoteStatus(p_quarter->C, &SERIAL_PORT); SERIAL_PORT.println();
      Staff::printNoteStatus(p_quarter->B, &SERIAL_PORT); SERIAL_PORT.println();
      Staff::printNoteStatus(p_quarter->A, &SERIAL_PORT); SERIAL_PORT.println();
      
      SERIAL_PORT.println();
      delay(100); // a pause between quarter notes
    }
    SERIAL_PORT.println();
    SERIAL_PORT.println();
  }
  SERIAL_PORT.println();
  SERIAL_PORT.println();
  SERIAL_PORT.println();

  delay(1000);
  
}

inline void debug_print(const char* msg){
  #ifdef DEBUG
    SERIAL_PORT.print(msg);
  #endif
}
