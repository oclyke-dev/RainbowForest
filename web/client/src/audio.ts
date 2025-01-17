import * as banjo from './assets/audio/banjo/banjo';
import * as flute from './assets/audio/flute/flute';
import * as cello from './assets/audio/cello/cello';
import * as guitar from './assets/audio/guitar/guitar';
import * as trombone from './assets/audio/trombone/trombone';
import * as violin from './assets/audio/violin/violin';
import * as percussion from './assets/audio/percussion/percussion';

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
  undefined,                        // 0
  loadInstrument(banjo.notes),      // 1 (red)
  loadInstrument(trombone.notes),   // 2 (yellow)
  loadInstrument(flute.notes),      // 3 (green)
  loadInstrument(percussion.notes), // 4 (cyan)
  loadInstrument(violin.notes),     // 5 (blue)
  loadInstrument(cello.notes),      // 6 -- UNUSED --
  loadInstrument(guitar.notes),     // 7 -- UNUSED --
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

// change volume
export const default_gain = 1.0;
export const min_gain = 0.0;
export const max_gain = 2.0;


export const setGain = (gain: number) => {
  if(gain > max_gain){ gain = max_gain; }
  if(gain < min_gain){ gain = min_gain; }
  main_level_ctrl.gain.setValueAtTime(gain, 0);
  return gain;
}

// adjust playback speed
export const min_bpm = 60;
export const max_bpm = 180;

export let period = 750;
export const setBPM = (bpm: number) => {
  if(bpm > max_bpm){ bpm = max_bpm; }
  if(bpm < min_bpm){ bpm = min_bpm; }
  period = 60*1000/bpm;
  return bpm;
}
