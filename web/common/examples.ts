/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

export type ExampleID = '1' | '2' | '3' | '4' | '5';
type Example = {
  name: string,
  colors: (string | null)[],
};

const N = null;
const T = 'ffff00';
const F = '00ff00';
const P = '0000ff';
const G = 'ff0000';

export const examples: Example[] = [
  {
    name: 'axelf',
    colors: [
      /*           0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15 */
      /* 6 (B) */  N, N, N, N, N, N, N, N, N, N, T, N, N, N, T, N,
      /* 5 (A) */  N, N, N, N, N, T, T, N, N, N, N, N, N, N, N, N,
      /* 4 (G) */  N, N, T, N, N, N, N, N, N, N, N, N, N, N, N, T,
      /* 3 (F) */  N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
      /* 2 (E) */  T, N, N, N, T, N, N, N, T, N, N, N, T, N, N, N,
      /* 1 (D) */  N, N, N, N, N, N, N, T, N, N, N, N, N, N, N, N,
      /* 0 (C) */  N, N, N, N, N, N, N, N, N, N, N, N, N, T, N, N, 
      ]
  },
  {
    name: 'pokemon level up',
    colors: [
      /*           0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15 */
      /* 6 (B) */  N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
      /* 5 (A) */  N, N, N, N, N, N, N, N, G, G, G, N, N, N, N, N,
      /* 4 (G) */  G, N, N, N, G, G, G, N, N, N, N, N, N, G, G, G,
      /* 3 (F) */  N, G, N, G, N, N, N, N, N, N, N, N, N, N, N, N,
      /* 2 (E) */  N, N, G, N, N, N, N, N, N, N, N, N, N, N, N, N,
      /* 1 (D) */  N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
      /* 0 (C) */  N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, 
    ]
  },
  {
    name: 'funkytown',
    colors: [
      /*           0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15 */
      /* 6 (B) */  N, N, N, N, N, N, N, N, N, N, F, N, N, N, N, N,
      /* 5 (A) */  N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
      /* 4 (G) */  F, F, N, F, N, N, N, N, F, N, N, F, N, N, N, N,
      /* 3 (F) */  N, N, F, N, N, N, N, N, N, N, N, N, N, N, N, N,
      /* 2 (E) */  N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
      /* 1 (D) */  N, N, N, N, N, F, N, F, N, N, N, N, N, N, N, N,
      /* 0 (C) */  N, N, N, N, N, N, N, N, N, F, N, N, N, N, N, N, 
    ]
  },
  {
    name: 'mii shop channel',
    colors: [
      /*           0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15 */
      /* 6 (B) */  N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
      /* 5 (A) */  N, N, P, N, N, P, N, N, N, N, N, N, N, N, N, N,
      /* 4 (G) */  N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
      /* 3 (F) */  P, N, N, N, N, N, N, P, N, N, N, N, N, N, N, N,
      /* 2 (E) */  N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
      /* 1 (D) */  N, N, N, P, N, N, N, N, P, P, P, N, N, N, N, N,
      /* 0 (C) */  N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, 
    ]
  },
  {
    name: 'somewhere over the rainbow',
    colors: [
      /*           0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15 */
      /* 6 (B) */  N, N, N, N, N, N, N, N, F, N, N, N, F, N, N, N,
      /* 5 (A) */  N, N, N, N, N, N, N, N, N, N, N, F, N, N, N, N,
      /* 4 (G) */  F, N, N, N, N, N, N, N, N, N, F, N, N, N, N, N,
      /* 3 (F) */  N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
      /* 2 (E) */  N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
      /* 1 (D) */  N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
      /* 0 (C) */  N, N, N, N, F, N, N, N, N, N, N, N, N, N, F, N, 
    ]
  },
  {
    name: 'jurassic park',
    colors: [
      /*           0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15 */
      /* 6 (B) */  T, N, T, N, N, N, N, N, T, N, T, N, N, N, N, N,
      /* 5 (A) */  N, T, N, N, N, N, N, N, N, T, N, N, N, N, N, N,
      /* 4 (G) */  N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
      /* 3 (F) */  N, N, N, N, T, N, N, N, N, N, N, N, T, N, N, N,
      /* 2 (E) */  N, N, N, N, N, N, T, N, N, N, N, N, N, N, T, N,
      /* 1 (D) */  N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
      /* 0 (C) */  N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, 
    ]
  },
];
