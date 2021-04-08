/*
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import {useState, useEffect, RefObject} from 'react';

import {getRect} from './useRect';

export const useReflowed = <T extends HTMLElement>(ref: RefObject<T>) => {
  const [reflowed, setReflowed] = useState<boolean>(false);

  // this hook will ensure that a ref is valid according to this function
  const valid = (ref: RefObject<T>): boolean => {
    return getRect(ref).width !== 0;
  }

  // using requestAnimationFrame per: https://swizec.com/blog/how-to-wait-for-dom-elements-to-show-up-in-modern-browsers/
  const waitForReflow = () => {
    if(valid(ref)){
      setReflowed(true);
    }else{
      window.requestAnimationFrame(waitForReflow);
    }
  }

  // this technique is only used for the initial render
  useEffect(() => {
    waitForReflow();
    return undefined; // no cleanup function
  }, []);

  return reflowed;
}
