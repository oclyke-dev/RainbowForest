/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import {
  HSV,
} from './../../../common/gradient';

export const validateHSV = (src: HSV[]): HSV[] => {
  return src.map(c => {
    let y = {...c}; // break the reference to the parent array

    if(typeof(y.h) === 'undefined'){ y.h = 0; }
    if(typeof(y.s) === 'undefined'){ y.s = 0; }
    if(typeof(y.v) === 'undefined'){ y.v = 0; }

    while(y.h >= 360){ y.h -= 360 };
    while(y.h  < 0){ y.h += 360 };

    if(y.s >= 100){ y.s = 100; }
    if(y.s  <   0){ y.s =   0; }
    if(y.v >= 100){ y.v = 100; }
    if(y.v  <   0){ y.v =   0; }

    return y;
  });
}
