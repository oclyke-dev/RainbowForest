#ifndef _STAFF_H_
#define _STAFF_H_

#include "Arduino.h"

typedef enum {
  NOTE_STATUS_NONE = 0x00,
  NOTE_STATUS_RED,
  NOTE_STATUS_GREEN,
  NOTE_STATUS_BLUE,
  NOTE_STATUS_YELLOW,
  NOTE_STATUS_CYAN,
  NOTE_STATUS_MAGENTA,

  // number of note statuses (can be used to check validity)
  NOTE_STATUS_NUM,
  NOTE_STATUS_ERROR
} note_status_e;  // note statuses will be directly transferred by the array sensor
typedef uint8_t note_status_t;
void printNoteStatus( note_status_t stat );

typedef struct _quarter_bar_t {
  note_status_t A;
  note_status_t B;
  note_status_t C;
  note_status_t D;
  note_status_t E;
  note_status_t F;
  note_status_t G;
} quarter_bar_t; // we are stuck with 4 notes per bar (measure) so this quarter-bar type indicates the values of all the notes at each position;
const quarter_bar_t qb_zero = {0};

typedef struct _bar_t {
  quarter_bar_t q_1;
  quarter_bar_t q_2;
  quarter_bar_t q_3;
  quarter_bar_t q_4;
} bar_t; // each bar has 4 quarter bars
const bar_t b_zero = {0};

class Staff {
  private:
    uint32_t  _len_bars = 0;  // length of this Staff in bars
    bar_t*    _p_bars = NULL; // pointer to bar data
  protected:
  public:
    Staff(uint32_t len_bars); // Constructor
    ~Staff( void );           // Destructor

    uint32_t      getLengthBars( void );
    
    note_status_t getNoteStatus(uint32_t bar_index, uint8_t quarter_bar_index, uint8_t note_index);
    note_status_t setNoteStatus(note_status_t stat, uint32_t bar_index, uint8_t quarter_bar_index, uint8_t note_index);

    bar_t*        getBarsPointer( void );

    void          printInfo( Stream* p_stream );
    static void   printNoteStatus( note_status_t stat, Stream* p_stream );
    
};

#endif // _STAFF_H_
