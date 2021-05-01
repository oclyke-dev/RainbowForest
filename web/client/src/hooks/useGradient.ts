/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import {
  useState,
  useEffect,
} from 'react';

import {
  Gradient,
} from './../../../common/gradient';

let gradient: Gradient = [];
export const useGradient = (): [Gradient, React.Dispatch<React.SetStateAction<Gradient>>] => {
  const [state_gradient, setGradient] = useState<Gradient>(gradient);
  gradient = state_gradient; // doing this allows users to change the gradient via state, but always returns the one true gradient
  return [gradient, setGradient];
}
