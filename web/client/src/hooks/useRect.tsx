/*
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import { useState, useEffect, RefObject } from 'react';

type RectOptions = {parent?: boolean};

export const getRect = <T extends HTMLElement>(ref: RefObject<T>, options?: RectOptions): DOMRect => {
  const fallback = new DOMRect(0, 0, 0, 0);
  if(ref === null){ return fallback; }
  if(ref.current === null){ return fallback; }

  if(typeof(options) === 'undefined'){
    options = {}
  }

  const target = (options.parent) ? ref.current.parentElement : ref.current;
  if(target === null){ return fallback; }

  return target.getBoundingClientRect();
}

export const useRect = <T extends HTMLElement>(ref: RefObject<T>, options?: RectOptions): DOMRect => {
  const [prev, setPrev] = useState(ref.current); 
  const [rect, setRect] = useState(getRect(ref, options));

  // use an edge trigger to update rect when refs become available
  if((prev === null) && (ref.current)){
    setPrev(ref.current);
    setRect(getRect(ref, options));
  }

  // use a one-time effect to add a resize handler, and to clean it up when unmounting
  useEffect(() => {
    // update rect on resize
    const resizeHandler = () => { setRect(getRect(ref, options)); }
    window.addEventListener('resize', resizeHandler);

    // return the cleanup function
    return () => { window.removeEventListener('resize', resizeHandler); };
  }, []);

  return rect;
} 
