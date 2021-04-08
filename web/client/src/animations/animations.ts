/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

export type StaffAnimation = FrameRequestCallback | undefined;

import scroll from './scroll';
import diamond from './diamond';
import diagonal from './diagonal';

export type AnimationFormat = {ani?: (ts: number, dims: {width: number, height: number}) => string[][], name: string};
export const animations: AnimationFormat[] = [
  {name: 'none', ani: undefined},
  scroll,
  diagonal,
  diamond,
]

let current_animation: StaffAnimation = undefined;
let request: number | undefined = undefined;
const animation_fps = 10;
const fps_period = 1000 / animation_fps;
let last_draw = Date.now();

const animationDriver = (timestamp: number) => {
  if(current_animation){
    const now = Date.now();
    const elapsed = now - last_draw;
    if(elapsed > fps_period){
      last_draw = now;
      current_animation(timestamp); // call the desired fn
    }
    request = window.requestAnimationFrame(animationDriver);
  }
}

export const setAnimation = (ani: StaffAnimation) => {

  if(typeof ani === 'undefined'){
    current_animation = undefined;
    request = undefined;
  }else{
    current_animation = ani;
    if(!request){
      request = window.requestAnimationFrame(animationDriver);
    }
  }
}

export type HSV = {
  h: number,
  s: number,
  v: number,
}

