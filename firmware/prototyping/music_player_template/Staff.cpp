#include "Staff.h"

Staff::Staff(uint32_t len_bars){  // Constructor
  bar_t* p_temp = (bar_t*)malloc(len_bars*sizeof(bar_t));
  if(p_temp == NULL){ return; }
  _len_bars = len_bars;
  _p_bars = p_temp;
  memset((void*)_p_bars, 0x00, _len_bars*4*sizeof(quarter_bar_t));
}

Staff::~Staff( void ){
  if(_p_bars != NULL){
    free(_p_bars); 
    _p_bars = NULL;
    _len_bars = 0;
  }
}

uint32_t Staff::getLengthBars( void ){
  return _len_bars;
}

note_status_t Staff::getNoteStatus(uint32_t bar_index, uint8_t quarter_bar_index, uint8_t note_index){
  note_status_t retval = NOTE_STATUS_ERROR;
  if(_p_bars == NULL){ return retval; }
  if(bar_index >= _len_bars){ return retval; }
  bar_t* p_bar = (_p_bars + bar_index);
  quarter_bar_t* p_quarter_bar = ((quarter_bar_t*)p_bar + quarter_bar_index);

  switch(note_index){
    case 0: retval = p_quarter_bar->A; break;
    case 1: retval = p_quarter_bar->B; break;
    case 2: retval = p_quarter_bar->C; break;
    case 3: retval = p_quarter_bar->D; break;
    case 4: retval = p_quarter_bar->E; break;
    case 5: retval = p_quarter_bar->F; break;
    case 6: retval = p_quarter_bar->G; break;
  }

  return retval;
}

note_status_t Staff::setNoteStatus(note_status_t stat, uint32_t bar_index, uint8_t quarter_bar_index, uint8_t note_index){
  if(_p_bars == NULL){ return NOTE_STATUS_ERROR; }
  if(bar_index >= _len_bars){ return NOTE_STATUS_ERROR; }
  bar_t* p_bar = (_p_bars + bar_index);
  quarter_bar_t* p_quarter_bar = ((quarter_bar_t*)p_bar + quarter_bar_index);

  switch(note_index){
    case 0: p_quarter_bar->A = stat; break;
    case 1: p_quarter_bar->B = stat; break;
    case 2: p_quarter_bar->C = stat; break;
    case 3: p_quarter_bar->D = stat; break;
    case 4: p_quarter_bar->E = stat; break;
    case 5: p_quarter_bar->F = stat; break;
    case 6: p_quarter_bar->G = stat; break;
    default: return NOTE_STATUS_ERROR; break;
  }
  
  return stat;
}

bar_t* Staff::getBarsPointer( void ){
  return _p_bars;
}


void Staff::printNoteStatus( note_status_t stat, Stream* p_stream ){
  switch(stat){
    case NOTE_STATUS_NONE :     p_stream->print('-'); break;
    case NOTE_STATUS_RED :      p_stream->print('R'); break;
    case NOTE_STATUS_GREEN :    p_stream->print('G'); break;
    case NOTE_STATUS_BLUE :     p_stream->print('B'); break;
    case NOTE_STATUS_YELLOW :   p_stream->print('Y'); break;
    case NOTE_STATUS_CYAN :     p_stream->print('C'); break;
    case NOTE_STATUS_MAGENTA :  p_stream->print('M'); break;
    default :                   p_stream->print('?'); break;
  }
}

void Staff::printInfo( Stream* p_stream ){
  for(uint8_t in = 7; in > 0; in--){                            // for each note
    for(uint32_t ib = 0; ib < _len_bars; ib++){                 // for each bar
      for(uint8_t iqb = 0; iqb < 4; iqb++){                     // for each quarter bar
        printNoteStatus(getNoteStatus(ib, iqb, in-1), p_stream);
      }
      p_stream->print(" | ");
    }
    p_stream->println();
  }
  p_stream->println();
}
