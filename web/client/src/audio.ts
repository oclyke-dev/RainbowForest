import * as banjo from './assets/audio/banjo/banjo';
import * as flute from './assets/audio/flute/flute';
import * as cello from './assets/audio/cello/cello';
import * as guitar from './assets/audio/guitar/guitar';
import * as trombone from './assets/audio/trombone/trombone';
import * as violin from './assets/audio/violin/violin';

// types
export type Instrument = {
  notes: {
    url: string,
    buffer: AudioBuffer | undefined,
  }[],
}

// load instruments
const loadInstrument = async (urls: string[]) => {
  let instrument: Instrument = {
    notes: urls.map((url) => ({url: url, buffer: undefined})),
  };

  urls.forEach((url, idx) => {
    window.fetch(url)
    .then(response => response.arrayBuffer())
    .then(arrayBuffer => context.decodeAudioData(arrayBuffer))
    .then(audioBuffer => {
      instrument.notes[idx].buffer = audioBuffer;
    })
    .catch(e => console.warn(e));
  })
  
  return instrument;
}

let instruments = [
  undefined,                      // 0
  loadInstrument(banjo.notes),    // 1
  loadInstrument(flute.notes),    // 2
  loadInstrument(cello.notes),    // 3
  loadInstrument(guitar.notes),   // 4
  loadInstrument(trombone.notes), // 5
  loadInstrument(violin.notes),   // 6
]


// main gain
const context = new AudioContext();
const main_level_ctrl = context.createGain();
main_level_ctrl.gain.setValueAtTime(0.99, 0);
main_level_ctrl.connect(context.destination);


// play a column
export const playColumn = (col: number[]) => {

  col.forEach((c, note) => {
    const promise = instruments[c];
    if(typeof promise !== 'undefined'){
      promise.then((instrument) => {
        const source = context.createBufferSource();
        const buffer = instrument.notes[note].buffer;
        if(buffer){
          source.buffer = buffer;
          source.connect(main_level_ctrl);
          source.start();
        }
      })
    }
  })
}



