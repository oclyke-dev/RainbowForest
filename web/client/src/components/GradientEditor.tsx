/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import React, {
  useRef,
  useState,
  useEffect,
} from 'react';

import {
  Box,
  IconButton,
} from '@material-ui/core';
import {
  makeStyles,
} from '@material-ui/core/styles';
import CloseIcon from '@material-ui/icons/Close';

import {
  makeCSSGradient,
} from './../utilities/cssgradient';

import {
  useGradient,
} from '../hooks/useGradient';

import {
  useRect,
} from './../hooks/useRect';

import {
  useReflowed
} from './../hooks/useReflowed';

import {
  HSV,
  Gradient,
  sampleGradient_HSV,
  sampleGradient_RGB,
} from './../../../common/gradient';

const convert = require('color-convert')

const useStyles = makeStyles((theme) => ({
  root: {
    display: 'flex',
    flexDirection: 'row',
    flexGrow: 1,

  },
  splash: {
    border: '3px solid whitesmoke',
    borderRadius: '20px',

    width: '100%',

    cursor: 'copy',
  },
  handle: {
    position: 'absolute',
    transform: 'translate(-50%, -50%)',

    border: '3px solid whitesmoke',
    width: '5px',
    borderRadius: '20px',

    cursor: 'ew-resize',
  },
  x: {
    position: 'relative',
    transform: 'translate(-50%, -50%)',
    borderRadius: '50%',
    left: '50%',
    top: '140%',
    // backgroundColor: 'black',
  }
}));

let selected: number | undefined = undefined;

const GradientEditor = () => {
  const classes = useStyles();

  // ensure positioning on page is valid
  const self = useRef<HTMLDivElement>(null);
  const bounds = useRect(self);
  useReflowed(self);

  const [gradient, setGradient] = useGradient();

  const select = (idx: number) => {
    selected = idx;
    const hsv = gradient[selected].color;
    const hex = convert.hsv.hex([hsv.h, hsv.s, hsv.v]);

    // setPickerColor(hex);
    const event = new CustomEvent('gradient-selection-change', {detail: {selected, color: hex}});
    window.dispatchEvent(event);
  }

  const unselect = () => {
    selected = undefined;
  }

  const sendGradientUpdateEvent = (g: Gradient) => {
    window.dispatchEvent(new CustomEvent('gradient-change', {detail: g}));
  }

  const handleDrag = (e: PointerEvent) => {
    const pos = {x: e.pageX, y: e.pageY};
    let loc = (pos.x - bounds.left) / bounds.width;
    if(loc < 0){ loc = 0; }
    if(loc > 1){ loc = 1; }

    setGradient(prev => {
      if(typeof selected === 'undefined'){ return prev }
      let current = [...prev];
      let prev_el = prev[selected];
      current[selected] = {
        ...prev_el,
        loc,
      }
      sendGradientUpdateEvent(current);
      return current;
    });
  }

  const handleUp = () => {
    window.removeEventListener('pointermove', handleDrag);
    window.removeEventListener('pointerup', handleUp);
  }

  // set up a listener for color changes by the color picker
  useEffect(() => {
    const handlePickerColorChange = (e: any) => {
      const complete = e.detail.complete;
      const hex = e.detail.color.hex;
      const hsv_arr = convert.hex.hsv(hex);
      const hsv: HSV = {h: hsv_arr[0], s: hsv_arr[1], v: hsv_arr[2]};

      // here's where we can change the gradient index to match
      setGradient(prev => {
        if(typeof selected === 'undefined'){ return prev }
        let current = [...prev];
        current[selected] = {
          ...prev[selected],
          color: hsv,
        }
        if(complete){
          sendGradientUpdateEvent(current);
        }
        return current;
      });
    }
    window.addEventListener('color-picker-change', handlePickerColorChange);

    return () => {
      window.removeEventListener('color-picker-change', handlePickerColorChange);
    }
  }, []);

  return <>
    <div ref={self} className={classes.root}>
      <Box
        className={classes.splash}
        style={{background: makeCSSGradient(gradient)}} 
        onClick={(e) => {
          const pos = {x: e.pageX, y: e.pageY};
          let loc = (pos.x - bounds.left) / bounds.width;
          if(loc < 0){ loc = 0; }
          if(loc > 1){ loc = 1; }

          setGradient(prev => {
            const current = [...prev];
            current.push({color: sampleGradient_HSV(gradient, loc), loc, flags: {hover: false}});
            sendGradientUpdateEvent(current);
            return current;
          });
        }}
      />



    {gradient.map((e, idx) => {

      const c = e.color;

      const style = {
        height: bounds.height,
        left: bounds.left + e.loc*bounds.width,
        top: bounds.top + bounds.height/2,
        backgroundColor: convert.hsv.hex([c.h, c.s, c.v]),
      }

      return <React.Fragment key={`gradient_editor.handle.${idx}`}>
        <Box 
          className={classes.handle}
          style={style} 
          onMouseOver={() => {
            // console.log('mouseover', idx)
          }}
          onMouseOut={() => {
            // console.log('mouseout', idx)
          }}
          onPointerDown={() => {
            select(idx);
            window.addEventListener('pointermove', handleDrag);
            window.addEventListener('pointerup', handleUp);
          }}
        >
      {selected === idx && <>
        <IconButton 
          className={classes.x}
          size='small'
          onClick={() => {
            unselect();
            setGradient(prev => {
              if(prev.length === 2){
                return prev;
              }
              let current = [...prev];
              current.splice(idx, 1);
              sendGradientUpdateEvent(current);
              return current;
            });            
          }}
        >
          <CloseIcon />
        </IconButton>
      </>}

        </Box>
      </React.Fragment>
    })}      
    </div>
  </>
}

export default GradientEditor;
