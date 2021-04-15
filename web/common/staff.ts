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

type StaffEvents = 'change';
type EventHandler = (staff: Staff) => void;

export class Staff {
  readonly width: number;
  readonly height: number;
  private _data: StaffData;

  handlers: {
    [E in StaffEvents]?: EventHandler[]
  }

  constructor (width: number, height: number, depth: number) {
    this.width = width;
    this.height = height;
    this._data = [...new Array(this.width)].map(() => {
      return [...new Array(this.height)].map(() => {
        return {
          color: '#ffffff',
          note: 0,
        }
      })
    });

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

  column (idx: number) {
    return this._data[idx]
  }

  at (idc: number, idr: number) {
    return this.column(idc)[idr];
  }

  row (idx: number) {
    return [...new Array(this.width)].map((e, idc) => { return this.column(idc)[idx]; })
  }

  data () {
    return this._data;
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
    }
  }
}

// staff
const staff = new Staff(16, 7, 5); // set staff dimensions here

export default staff;
