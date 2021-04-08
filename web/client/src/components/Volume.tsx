/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import {
  useState,
  useEffect,
} from 'react';

import {
  Box,
  Button,
  Divider,
  Tooltip,
  IconButton,
  Grid,
  Slider,
  Popover,
} from '@material-ui/core';
import {
  makeStyles,
} from '@material-ui/core/styles';

import VolumeDown from '@material-ui/icons/VolumeDown';
import VolumeUp from '@material-ui/icons/VolumeUp';

import {
  setGain as applyGain,
  min_gain,
  max_gain,
} from './../audio';


const useStyles = makeStyles((theme) => ({
  button: {
  },
  slider: {
    // width: '25%',
    height: '150px',
    padding: theme.spacing(1),
  },
}));


const Volume = () => {
  const classes = useStyles();

  const [volume_anchor_el, setVolumeAnchorEl] = useState<HTMLButtonElement | null>(null);
  const [gain, setGain] = useState<number>(1.0);

  return <>


    <Box className={classes.button} display='flex' flexDirection='column' justifyContent='space-around' height='100%'>
      <IconButton 
        onClick={(e) => {
          setVolumeAnchorEl(e.currentTarget);
        }}
      >
        {(gain >= ((min_gain + max_gain) / 2)) ? <VolumeUp /> : <VolumeDown />}
      </IconButton>
    </Box>
    <Popover
      open={Boolean(volume_anchor_el)}
      anchorEl={volume_anchor_el}
      onClose={() => {
        setVolumeAnchorEl(null);
      }}
      anchorOrigin={{
        vertical: 'top',
        horizontal: 'center',
      }}
      transformOrigin={{
        vertical: 'bottom',
        horizontal: 'center',
      }}
    >
      <Box className={classes.slider}>
        <Slider
          value={gain}
          min={min_gain}
          max={max_gain}
          step={Math.abs(max_gain - min_gain)/1000}
          orientation='vertical'
          onChange={(e, value) => {
            if(!Array.isArray(value)){
              setGain(applyGain(value));
            }
          }}
        />
      </Box>
    </Popover>
  </>
}

export default Volume;
