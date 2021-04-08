/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import {
  useState,
  useEffect,
} from 'react';

import {
  truth
} from './../../../common/secrets';

export const useAuth = (): [boolean, ((key: string) => void)] => {
  const [authenticated, setAuthenticated] = useState<boolean>(false);
  
  // get initial auth status
  useEffect(() => {
    const key = localStorage.getItem('key');
    setAuthenticated(key === truth);
  }, []);

  // allow user to modify auth
  const setAuthKey = (key: string) => {
    localStorage.setItem('key', key);
    setAuthenticated(key === truth);
  }
  
  return [authenticated, setAuthKey];
}
