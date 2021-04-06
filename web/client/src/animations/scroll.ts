/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import {
  AnimationFormat,
} from './animations';

import {
  sampleGradient_HSV,
  sampleGradient_RGB, 
} from '../hooks/useGradient';

var convert = require('color-convert');

const scroll: AnimationFormat = {
  name: 'scroll',
  ani: (ts, dims) => {

    const max_freq = 0.1;
    const seconds = (ts/1000);
    const phase_norm = (seconds*max_freq) % 1;
    const phase = (1-phase_norm);

    let colors = [...new Array(dims.width)].map((col, x) => {
      return [...new Array(dims.height)].map((row, y) => {

        const lrp = x/dims.width;
        const tp = (lrp + phase) % 1;
        const c = sampleGradient_RGB(tp);

        return convert.rgb.hex([c.r, c.g, c.b]);
      });
    })
    return colors;
  }
};

export default scroll;
