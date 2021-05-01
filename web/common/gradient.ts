/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

const convert = require('color-convert');

export type HSV = {
  h: number,
  s: number,
  v: number,
}

export type GradientEntry = {
  color: HSV,
  loc: number, // [0, 1]
  flags: {
    hover: boolean,
  }
}

export type Gradient = GradientEntry[];


const initial_gradient_elements = 5;
export const initial_gradient: Gradient =[...new Array(initial_gradient_elements)].map((e, idx) => {
  return {
    color: {
      h: 360*(idx/initial_gradient_elements),
      s: 100,
      v: 100,
    },
    loc: idx/(initial_gradient_elements - 1),
    flags: {
      hover: false,
    }
  }
});


const getLowerHigherIDX = (g: Gradient, tp: number) => {
  if(g.length === 0 || g.length < 2){
    console.error('this should not happen oh no!');
    throw new Error();
  }

  let highest_lower_idx = 0;
  let lowest_higher_idx = g.length - 1;

  g.forEach((e, idx) => {
    if(e.loc < tp){
      if(highest_lower_idx < idx){
        highest_lower_idx = idx;
      }
    }
    if(e.loc > tp){
      if(lowest_higher_idx > idx){
        lowest_higher_idx = idx;
      }
    }
  });

  return [lowest_higher_idx, highest_lower_idx];
}

export const sampleGradient_HSV = (g: Gradient, tp: number): HSV => {
  const [lowest_higher_idx, highest_lower_idx] = getLowerHigherIDX(g, tp);

  if(lowest_higher_idx === highest_lower_idx){
    return g[highest_lower_idx].color;
  }

  const upper = g[lowest_higher_idx].color;
  const lower = g[highest_lower_idx].color;
  
  const upper_ref = g[lowest_higher_idx].loc;
  const lower_ref = g[highest_lower_idx].loc;

  return {
    h: (upper.h - lower.h)/(upper_ref - lower_ref) * (tp - lower_ref) + lower.h,
    s: (upper.s - lower.s)/(upper_ref - lower_ref) * (tp - lower_ref) + lower.s,
    v: (upper.v - lower.v)/(upper_ref - lower_ref) * (tp - lower_ref) + lower.v,
  }
}

export const sampleGradient_RGB = (g: Gradient, tp: number): {r: number, g: number, b: number} => {
  const [lowest_higher_idx, highest_lower_idx] = getLowerHigherIDX(g, tp);

  if(lowest_higher_idx === highest_lower_idx){
    const hsv = g[highest_lower_idx].color;
    const rgb = convert.hsv.rgb([hsv.h, hsv.s, hsv.v])
    return {
      r: rgb[0],
      g: rgb[1],
      b: rgb[2],
    };
  }

  const upper_hsv = g[lowest_higher_idx].color;
  const lower_hsv = g[highest_lower_idx].color;

  const upper_rgb = convert.hsv.rgb([upper_hsv.h, upper_hsv.s, upper_hsv.v]);
  const lower_rgb = convert.hsv.rgb([lower_hsv.h, lower_hsv.s, lower_hsv.v]);

  const upper = {r: upper_rgb[0], g: upper_rgb[1], b: upper_rgb[2]};
  const lower = {r: lower_rgb[0], g: lower_rgb[1], b: lower_rgb[2]};
  
  const upper_ref = g[lowest_higher_idx].loc;
  const lower_ref = g[highest_lower_idx].loc;

  return {
    r: (upper.r - lower.r)/(upper_ref - lower_ref) * (tp - lower_ref) + lower.r,
    g: (upper.g - lower.g)/(upper_ref - lower_ref) * (tp - lower_ref) + lower.g,
    b: (upper.b - lower.b)/(upper_ref - lower_ref) * (tp - lower_ref) + lower.b,
  }
}

