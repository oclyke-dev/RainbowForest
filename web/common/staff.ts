/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import { debug } from './utility';
import Message, {
  EntryData,
  StaffData,
  ColumnFormat,
  EntryFormat,
  StaffFormat,
} from './message';

import {
  examples,
} from './examples';

import {
  Gradient,
  initial_gradient,
} from './gradient';

type StaffEvents = 'change';
type EventHandler = (staff: Staff) => void;

enum Color {
  off = '000000',
  white = 'ffffff',
};

const now = () => {
  return `todo: timestamp`;
}

export class Staff {
  readonly width: number;
  readonly height: number;
  private _data: StaffData;
  example: null | number;
  playhead: null | number;
  private _gradient: Gradient;


  handlers: {
    [E in StaffEvents]?: EventHandler[]
  }

  constructor (width: number, height: number, depth: number) {
    this.width = width;
    this.height = height;
    this._data = [...new Array(this.width)].map(() => {
      return [...new Array(this.height)].map(() => {
        return {
          color: Color.off,
          note: 0,
        }
      })
    });

    this.example = null;
    this.playhead = null;
    this._gradient = initial_gradient;

    this.handlers = {}
  }

  // on (key: StaffEvents, handler: EventHandler): void {
  //   if(!this.handlers[key]){
  //     this.handlers[key] = [];
  //   }
  //   (this.handlers[key] as any).push(handler);
  // }

  // dispatchEvent (key: StaffEvents) {
  //   this.handlers[key]?.forEach(h => h(this));
  // }

  gradient(){
    return this._gradient;
  }

  column (idx: number) {
    return this.data()[idx]
  }

  at (idc: number, idr: number) {
    return this.column(idc)[idr];
  }

  row (idx: number) {
    return [...new Array(this.width)].map((e, idc) => { return this.column(idc)[idx]; })
  }

  // this data accessor function allows us to 'compose' new colors on top of the stored node colors (for example to show the playhead and example songs...)
  data () {

    // expand without references!
    let composed = this._data.map(col => {
      return col.map(entry => {
        return {...entry}
      })
    });

    // add example song
    if(this.example !== null){
      let column = 0;
      let row = 0;
      examples[this.example].colors.forEach((c) => {
        if(c){
          composed[column][row] = {
            ...composed[column][row],
            color: c,
          }
        }

        column += 1;
        if(column >= this.width){
          row += 1;
          column = 0;
        }
      });
    }

    // add playhead
    if(this.playhead !== null){
      composed[this.playhead].forEach((e, idx) => {
        composed[(this.playhead as any)][idx] = {
          ...e,
          color: Color.white,
        }
      })
    }

    return composed;
  }

  setEntry (data: EntryFormat) {
    if(data.column >= this.width){ throw new TypeError(`invalid staff width: ${data.column} (maximum: ${this.width - 1})`); }
    if(data.row >= this.height){ throw new TypeError(`invalid staff height: ${data.row} (maximum: ${this.height - 1})`); }
    const entry = data.entry;

    // merge new data with existing
    let updated: EntryData = {...this._data[data.column][data.row]};
    if(entry.color){
      updated.color = entry.color;
    }
    if(typeof(entry.note) !== 'undefined'){
      updated.note = entry.note;
    }

    this._data[data.column][data.row] = updated;
    // this.dispatchEvent('change');
  }

  setColumn (data: ColumnFormat) {
    if(data.column >= this.width){ throw new TypeError(`invalid column. maximum ${this.width - 1} got ${data.column}`); }
    if(data.entries.length !== this.height){ throw new TypeError(`incorrect number of rows in column. expected ${this.height} got ${data.entries.length}`); }

    // use setEntry for merging data
    data.entries.forEach((e, idx) => {
      this.setEntry({column: data.column, row: idx, entry: e});
    });
    // this.dispatchEvent('change');
  }

  setStaff (data: StaffFormat) {
    if(data.length !== this.width){ throw new TypeError(`incorrect number of columns received. got ${data.length}, expected ${this.width}`); }
    data.forEach((c, idc) => {
      if(c.length !== this.height){ throw new TypeError(`incorrect number of rows in column ${idc}. expected ${this.height} got ${c.length}`); }
    });

    // use setColumn for merging data
    data.forEach((c, idx) => {
      this.setColumn({column: idx, entries: c});
    });
    // this.dispatchEvent('change');
  }

  // generate an update message in response
  handleRequest(msg: Message) {
    let response: Message = {
      id: {
        name: 'server',
      },
      timestamp: now(),
    }

    if(msg.request){
      response.update = {};
      if(msg.request.staff){
        response.update.staff = staff.data();
      }
      if(msg.request.columns){
        response.update.columns = [];
        msg.request.columns.forEach(c => {
          if(!response.update){ throw new TypeError('this should not happen - update'); }
          if(!response.update.columns){ throw new TypeError('this should not happen - columns'); }
          response.update.columns.push({column: c.column, entries: staff.column(c.column)});
        });
      }
      if(msg.request.entries){
        response.update.entries = [];
        msg.request.entries.forEach(e => {
          if(!response.update){ throw new TypeError('this should not happen - update'); }
          if(!response.update.entries){ throw new TypeError('this should not happen - entries'); }
          response.update.entries.push({column: e.column, row: e.row, entry: staff.at(e.column, e.row)});
        });
      }
      if(typeof msg.request.playhead !== 'undefined'){
        response.update.playhead = this.playhead;
      }
      if(typeof msg.request.example !== 'undefined'){
        response.update.example = this.example;
      }
      if(typeof msg.request.gradient !== 'undefined'){
        response.update.gradient = this.gradient();
      }
    }
    return response;
  }

  handleUpdate (msg: Message) {
    if(msg.update){
      if(msg.update.staff){
        this.setStaff(msg.update.staff);
      }
      if(msg.update.columns){
        msg.update.columns.forEach(c => {
          this.setColumn(c);
        });
      }
      if(msg.update.entries){
        msg.update.entries.forEach(e => {
          this.setEntry(e);
        });
      }
      if(typeof msg.update.playhead !== 'undefined'){
        this.playhead = msg.update.playhead;
      }
      if(typeof msg.update.example !== 'undefined'){
        this.example = msg.update.example;
      }
      if(msg.update.gradient){
        if(msg.update.gradient.length >= 2){
          this._gradient = msg.update.gradient;
        }
      }
    }
  }

  // send all staff data (public interface)
  makePublicUpdate() {
    const msg: Message = {
      id: {
        name: 'server',
      },
      timestamp: now(),
      update: {
        staff: staff.data(),
        gradient: this.gradient(),
      }
    };
    return msg;
  }
}

// staff
const staff = new Staff(16, 7, 5); // set staff dimensions here

export default staff;
