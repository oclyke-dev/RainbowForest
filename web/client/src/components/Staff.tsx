/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import React from 'react';
import {
  useState,
  useRef,
} from 'react';

import {
  Box,
  Button,
  Tooltip,
  Typography,
  Radio,
  RadioGroup,
  FormControl,
  FormControlLabel,
  FormLabel,
} from '@material-ui/core';
import {
  ToggleButton,
  ToggleButtonGroup,
} from '@material-ui/lab';
import {
  makeStyles,
} from '@material-ui/core/styles';

import {
  playColumn,
  period,
} from './../audio';

import MusicNoteIcon from '@material-ui/icons/MusicNote';
import PaletteIcon from '@material-ui/icons/Palette';

import SkipPreviousIcon from '@material-ui/icons/SkipPrevious';
import PlayArrowIcon from '@material-ui/icons/PlayArrow';
import PauseIcon from '@material-ui/icons/Pause';

import Volume from './../components/Volume';
import Speed from './../components/Speed';

import {
  setAnimation,
  animations,
} from '../animations/animations';

import {
  socket
} from './../utilities/interface';

import {
  useAuth,
} from './../hooks/useAuth';

import {
  Message,
  messageToString,
  ColumnFormat,
  EntryData,
} from './../../../common/message';

import {
  truth
} from './../../../common/secrets';

import GradientEditor from './GradientEditor';
import Picker from './Picker';

type Entry = {
  color: string,
  note: number,
}
export type StaffType = Entry[][];

type StaffProps = {
}

const useStyles = makeStyles((theme) => ({
  staff: {
    width: '100%',
    display: 'flex',
    flexDirection: 'row',
  },
  column: {
    padding: theme.spacing(0.1),
    // paddingRight: ''
  },
  note: {
    // create 1:1 aspect ratio with this css: https://stackoverflow.com/questions/5445491/height-equal-to-dynamic-width-css-fluid-layout
    height: 0,
    width: '100%',
    paddingBottom: '100%',

    // make a border
    borderStyle: 'solid',
    borderRadius: '50%',
    borderWidth: '1px',
    borderColor: theme.palette.grey[300],

    boxSizing: 'border-box',

    backgroundColor: 'green',
  },
  controlpane: {
    marginTop: theme.spacing(2),
    marginBottom: theme.spacing(2),
  },
  indicator: {
    
    padding: theme.spacing(0.1),
    height: theme.spacing(1),

    // make a border
    borderStyle: 'solid',
    borderRadius: theme.spacing(0.5),
    borderWidth: '1px',
    borderColor: theme.palette.grey[300],
  },
  highlight: {
    backgroundColor: theme.palette.primary.main,
  },
}));

type DisplayTypes = 'colors' | 'notes';
const note_color_map = ['white', 'red', 'green', 'blue', 'cyan', 'magenta'];

let timeout: NodeJS.Timeout;

type PlayState = {
  playing: boolean, // whether we are running or not
  next: number,     // next column to play
}
const initial_state: PlayState = {
  playing: false,
  next: 1,
};

const Staff = (props: StaffProps) => {
  const classes = useStyles();
  const [display, setDisplay] = React.useState<DisplayTypes>('notes');
  const [state, setState] = React.useState<PlayState>(initial_state);

  const [value, setValue] = React.useState('female');

  const handlePatternChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setValue((event.target as HTMLInputElement).value);
  };

  const [authorized] = useAuth();

  const [staff, setStaff] = useState<StaffType>([]);
  socket.onmessage = (e) => {
    const msg = JSON.parse(e.data) as any;
    // console.log(msg.update.staff)
    if(msg.update.staff){
      setStaff(msg.update.staff);
    }
  };
  const staff_ref = useRef(staff);
  staff_ref.current = staff;

  const total_cols = staff.length + 1;
  const col_width = `${(100/total_cols)}%`;

  const advancePlaying = () => {
    setState(prev => {
      let update = {...prev};
      const current = prev.next;
      
      playColumn(staff_ref.current[current].map(e => e.note));
      clearTimeout(timeout);
      timeout = setTimeout(advancePlaying, period);

      update.next = prev.next + 1;
      if(update.next >= staff_ref.current.length){
        update.next = 0;
      }
      return update;
    });
  }

  const resetPlaying = () => {
    // clearTimeout(timeout);
    setState((prev) => {
      return {
        ...initial_state,
        playing: prev.playing,
      }
    });
  }

  const togglePlaying = () => {
    let play = false;
    setState(prev => {
      clearTimeout(timeout);

      let update = {...prev};
      update.playing = !prev.playing;

      if(update.playing){
        play = true;
      }
      return update;
    });

    if(play){
      advancePlaying();
      timeout = setTimeout(advancePlaying, period);
    }
  }

  const setStaffColors = (colors: string[][]) => {
    if(colors.length !== staff.length){ return; }
    colors.forEach((e, idx) => {
      if(colors[idx].length !== staff[idx].length){ return; }
    });

    setStaff((prev) => {
      let current = [...prev];

      colors.forEach((c, idc) => {
        c.forEach((color, idr) => {
          current[idc][idr].color = color;
        });
      });

      if(authorized){
        // send updated colors to sensor columns
        let msg: Message = {
          timestamp: 'todo: timestamp',
          auth_key: truth,
          id: {
            name: 'client'
          },
          update: {
            columns: current.map((col, x) => ({column: x, entries: col})),
          }
        }
        socket.send(messageToString(msg));
      }

      return current;
    });
  }
  
  return <>

    {/* staff viewer */}
    <Box className={classes.staff}>
      <Box className={classes.column} style={{width: col_width, height: col_width}}>
      {[...new Array(staff[0]?.length)].map((e, idx) => {
        const scale = ['c', 'd', 'e', 'f', 'g', 'a', 'b'].reverse();
        const offset = 0;
        const note = scale[(idx + offset) % scale.length];
        return <Box key={`staff.column.scale.${note}`}className={classes.note} style={{backgroundColor: 'white', borderColor: 'white'}}>
          {note}
        </Box>;
      })}
      </Box>

    {staff.map((column, idc) => { 
      return <Box key={`staff.column.${idc}`} className={classes.column} style={{width: col_width, height: col_width}}>
      { column.map((entry, idr) => {

        const color = (display === 'colors') ? entry.color : note_color_map[entry.note]

        return <Box key={`staff.column.${idc}.row.${idr}`} className={classes.note} style={{backgroundColor: color}}>
        </Box> })}
      </Box> })}
    </Box>

    {/* progress indicator */}
    <Box>
      <Box className={classes.staff}>
        <Box className={classes.column} style={{width: col_width, height: col_width}}></Box>
      {staff.map((column, idc) => {
        const highlighted = (((state.next - 1) === idc));
        return <Box key={`staff.progress.${idc}`} className={`${classes.column} ${classes.indicator} ${highlighted && classes.highlight}`} style={{width: col_width}}>
          {/* {idc} */}
        </Box>
      })}
      </Box>
    </Box>

    <Box className={classes.controlpane} display='flex' flexDirection='row' justifyContent='space-between'>
        
    {(display === 'notes') && <>
      {/* play controls */}
      <Box display='flex' flexDirection='column' justifyContent='space-around'>
        <Box>
          <Button
            variant='outlined'
            aria-label='reset'
            size='small'
            onClick={resetPlaying}
          >
            <Tooltip title={'reset'} arrow >
              <SkipPreviousIcon/>
            </Tooltip>
          </Button>

          <Button
            variant='outlined'
            color={(state.playing) ? undefined : 'primary'}
            aria-label='play/pause'
            size='small'
            onClick={togglePlaying}
          >
            <Tooltip title={(state.playing) ? 'pause' : 'play'} arrow >
              {(state.playing) ? <PauseIcon /> : <PlayArrowIcon />}
            </Tooltip>
          </Button>
        </Box>
      </Box>

      {/* volume control */}
      <Volume/>

      {/* space taker */}
      <Box display='flex' flexGrow={1}>

        {/* speed control */}
        <Speed />
        
      </Box>
    </>}

      {/* gradient editor */}
    {(display === 'colors') && <>
      <Box display='flex' flexDirection='column' justifyContent='space-around' flexGrow={1}>
        <GradientEditor/>
      </Box>
    </>}

      
      {/* display toggle */}
      <Box display='flex' flexDirection='row' justifyContent='flex-end' marginLeft={2}>
        <Box display='flex' flexDirection='column' justifyContent='space-around'>
          <Box>
            <ToggleButtonGroup
              value={display}
              size='small'
              exclusive
              onChange={(e, update: DisplayTypes) => {
                if (update !== null) {
                  setDisplay(update);
                }
              }}
            >
              <ToggleButton value='notes' aria-label='show-notes'>
                <Tooltip title={'notes'} arrow>
                  <MusicNoteIcon fontSize='small'/>
                </Tooltip>
              </ToggleButton>

              <ToggleButton value='colors' aria-label='show-colors'>
                <Tooltip title={'colors'} arrow >
                  <PaletteIcon fontSize='small'/>
                </Tooltip>
              </ToggleButton>
            </ToggleButtonGroup>
          </Box>
        </Box>
      </Box>
    </Box>


  {(display === 'colors') && <>
    <Picker />



    <FormControl component='fieldset'>
      <FormLabel component='legend'>pattern</FormLabel>
      <RadioGroup row aria-label="gender" name="gender1" value={value} onChange={handlePatternChange}>
    {animations.map((a, idx) => {

      const handleSelect = () => {
        const animation = a.ani;
        if(animation){
          setAnimation((ts) => {
            setStaffColors(animation(ts, {width: staff.length, height: staff[0].length}));
          })
        }else{
          setAnimation(undefined);
        }
      }

      return <>
        <FormControlLabel value={a.name} control={<Radio onClick={handleSelect}/>} label={a.name} />
      </>
    })}
      </RadioGroup>
    </FormControl>

    <Box display='flex' flexWrap='wrap'>

    </Box>

  </>}

  </>
}

export default Staff;
