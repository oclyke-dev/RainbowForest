/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import {
  useRef,
  useState,
  useEffect,
} from 'react';

import {
  Box,
} from '@material-ui/core';

import {
  HuePicker,
  AlphaPicker,
  ColorChangeHandler,
} from 'react-color';


const Picker = () => {

  const [picker_color, setPickerColor] = useState<string>('#ffffff');

  // set up a listener for 
  useEffect(() => {
    const handleSelectionChange = (e: any) => {
      setPickerColor(e.detail.color);
    }
    window.addEventListener('gradient-selection-change', handleSelectionChange);
    return (() => {
      window.removeEventListener('gradient-selection-change', handleSelectionChange);
    });
  }, []);

  const signalColorChange: ColorChangeHandler  = (color, e) => {
    setPickerColor(color.hex);
    const event = new CustomEvent('color-picker-change', {detail: {color, complete: false}});
    window.dispatchEvent(event);
  }

  const signalColorChangeComplete: ColorChangeHandler  = (color, e) => {
    setPickerColor(color.hex);
    const event = new CustomEvent('color-picker-change', {detail: {color, complete: true}});
    window.dispatchEvent(event);
  }

  return <>
    <Box p={2} display='flex' flexDirection='row' justifyContent='space-around'>
      <Box>
        <HuePicker
          color={picker_color}
          onChange={signalColorChange}
          onChangeComplete={signalColorChangeComplete}
        />

      {/* <AlphaPicker
        color={picker_color}
        onChange={signalColorChange}
      /> */}
      </Box>
    </Box>
  </>
}

export default Picker;
