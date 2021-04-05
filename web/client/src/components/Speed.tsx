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
  Slider,
  TextField,
  InputAdornment,
} from '@material-ui/core';
import {
  makeStyles,
} from '@material-ui/core/styles';

import {
  setBPM as applyBPM,
  min_bpm,
  max_bpm,
} from './../audio';


const useStyles = makeStyles((theme) => ({
  root: {
    width: '100%',
    padding: theme.spacing(1),
  },
  input: {
    width: '75px',
  },
}));


const Speed = () => {
  const classes = useStyles();

  const [volume_anchor_el, setVolumeAnchorEl] = useState<HTMLButtonElement | null>(null);
  const [bpm, setBPM] = useState<number>(170);

  const [readout_bpm, setReadoutBPM] = useState(String(bpm));


  return <>


      <Box className={classes.root} display='flex' flexDirection='column' justifyContent='space-around'>
        <Box display='flex' flexDirection='row'>
        
          <Box className={classes.input}>
            <TextField
              value={readout_bpm}
              onChange={(e) => {
                setReadoutBPM(String(e.target.value));
                if(e.target.value === ''){ return; }

                const current = Number(e.target.value);
                if(current !== bpm){
                  setBPM(applyBPM(current));
                }
              }}
              onBlur={() => {
                const current = applyBPM(bpm);
                setReadoutBPM(String(current));
              }}
              onFocus={() => {
                setReadoutBPM('');
              }}
              InputProps={{
                startAdornment: <InputAdornment position="start">bpm</InputAdornment>,
              }}
            />
          </Box>

          <Box display='flex' flexGrow={1}>
            <Slider
              value={bpm}
              min={min_bpm}
              max={max_bpm}
              step={1}
              onChange={(e, value) => {
                if(!Array.isArray(value)){
                  const current = applyBPM(value);
                  setBPM(current);
                  setReadoutBPM(String(current));
                }
              }} 
            />
          </Box>
        </Box>
      </Box>
  </>
}

export default Speed;
