/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

export type EntryData = {
  color?: string,
  note?: number,
};
export type ColumnData = EntryData[];
export type StaffData = ColumnData[];

export type EntryFormat = {             // <-- may send individual entries
  row: number,
  column: number,
  entry: EntryData,
};
export type ColumnFormat = {            // <-- must send whole column (all entries)
  column: number,
  entries: ColumnData,
};
export type StaffFormat = ColumnData[]; // <-- must send all columns

export type Domain = {
  staff?: StaffFormat,
  columns?: ColumnFormat[],
  entries?: EntryFormat[],
}

export type ID = {
  name: 'column' | 'rpi' | 'server' | 'client',   // indicates who sent the message
  extra?: string,                         // a field to allow additional information (columns will provide their index)
}

export type Message = {
  timestamp: string,
  id: ID,
  auth_key?: string,
  update?: Domain,  // an updated state is being broadcast
  request?: Domain, // a device is requesting an update regarding a particular domain
}

export const messageToString = (msg: Message) => {
  return JSON.stringify(msg);
}

export const asMessage = (data: unknown): Message => {
  if (typeof data === 'object' && data !== null) {
    const maybeMessage = data as Message
    
    // check id
    const maybeID = maybeMessage.id as ID
    if(typeof maybeID.name !== 'string'){
      throw new TypeError('id.name is not a string');
    }
    if(maybeID.extra){
      if(typeof maybeID.extra !== 'string'){
        throw new TypeError('id.extra is not a string');
      }
    }
    
    // check timestamp
    if(typeof maybeMessage.timestamp !== 'string'){ throw new TypeError('timestamp not string type'); }

    // // check update format
    // if(maybeMessage.update){
    //   asDomain(maybeMessage.update);
    // }

    // // check set format
    // if(maybeMessage.set){
    //   asDomain(maybeMessage.set);
    // }

    // // check request format
    // if(maybeMessage.request){
    //   asDomain(maybeMessage.request);
    // }

    return maybeMessage;
  }
  throw new TypeError('data is not a Message');
}

export const strAsMessage = (str: string): Message => {
  const o = (JSON.parse(str) as unknown);
  return asMessage(o);
}

export default Message;
