/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import {
  HSV
} from './../animations/animations';

import {
  GradientEntry,
} from './../hooks/useGradient';

var convert = require('color-convert');

const hsvToStr = (c: HSV) => {
  const rgb = convert.hsv.rgb([c.h, c.s, c.v]);
  return `rgba(${rgb[0]}, ${rgb[1]}, ${rgb[2]}, 1)`;
}

export const makeCSSGradient = (cycle: GradientEntry[]) => {
  let code = '';
  let preamble = `linear-gradient(${90}deg,`;
  // let preamble = 'radial-gradient(circle,';
  let conclusion = ')';

  code = code.concat(preamble);

  // aha! i needed to break the reference that was persisting.... I am not trying to sort the original, just a local copy
  const sorted = [...cycle].sort((a, b) => {
    if(a.loc >= b.loc){ return 1; }
    return -1;
  });

  sorted.forEach((e, idx) => {
    const c = e.color;
    code = code.concat(hsvToStr(c));
    code = code.concat(`${100*(e.loc)}%`);
    if(idx < (cycle.length - 1)){
      code = code.concat(',');
    }
  });
  code = code.concat(conclusion);

  return code;
}
