/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

type DebugType = 'verbose' | 'log' | 'warn' | 'error';
type DebugLevel = DebugType | 'all';

const debug_lvl: DebugLevel = 'verbose';

const debug_fn_factory = (type: DebugType): (...args: any[]) => void => {
  let fn = (...args: any[]) => {};

  switch((debug_lvl as DebugLevel)){
    case 'error':
      // only error messages are shown
      if(type !== 'error'){
        return fn;
      }
      break;

    case 'warn':
      // errors and warnings are shown - logs and verbose are not
      if((type === 'log') || (type === 'verbose')){
        return fn; 
      }
      break;

    case 'log':
      // errors, warnings and logs are shown - verbose are not
      if(type === 'verbose'){
        return fn; 
      }
      break;
  }

  switch(type){
    case 'error': fn = console.log; break;
    case 'warn': fn = console.warn; break;

    // fall-through
    case 'verbose':
    case 'log': fn = console.log;
      break;
  }
  return fn;
}

export const debug = {
  error: debug_fn_factory('error'),
  warn: debug_fn_factory('warn'),
  log:  debug_fn_factory('log'),
  verbose:  debug_fn_factory('verbose'),
}