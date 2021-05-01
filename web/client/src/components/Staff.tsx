/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import React from 'react';
import {
  useState,
  useRef,
  useEffect,
} from 'react';

import {
  Box,
  Button,
  Tooltip,
} from '@material-ui/core';
import {
  ToggleButton,
  ToggleButtonGroup,
} from '@material-ui/lab';
import {
  makeStyles,
} from '@material-ui/core/styles';

import {
  playColumn as audioPlayColumn,
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
  socket
} from './../utilities/interface';

import {
  useAuth,
} from './../hooks/useAuth';

import {
  Message,
  messageToString,
  StaffFormat,
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

import {
  ExampleID,
} from './../../../common/examples';
import { useGradient } from '../hooks/useGradient';

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
const note_color_map = ['white', 'red', 'yellow', 'green', 'cyan', 'blue'];

let timeout: NodeJS.Timeout;

type PlayState = {
  playing: boolean, // whether we are running or not
  idx: number,     // column that is playing
}
const initial_state: PlayState = {
  playing: false,
  idx: 0,
};

let debounce = {
  toggle_playing: false,
  examples: false,
};

const Staff = (props: StaffProps) => {
  const classes = useStyles();
  const [display, setDisplay] = React.useState<DisplayTypes>('notes');
  const [state, setState] = React.useState<PlayState>(initial_state);
  const state_ref = useRef(state);
  state_ref.current = state;

  const [value, setValue] = React.useState('female');

  const handlePatternChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setValue((event.target as HTMLInputElement).value);
  };

  const [authorized] = useAuth();
  const authorized_ref = useRef(authorized);
  authorized_ref.current = authorized;

  const [gradient, setGradient] = useGradient();
  const gradient_ref = useRef(gradient);
  gradient_ref.current = gradient;

  const [staff, setStaff] = useState<StaffFormat>([]);
  socket.onmessage = (e) => {
    const msg = JSON.parse(e.data) as Message;
    if(msg.update){
      if(msg.update.staff){
        setStaff(msg.update.staff);
      }
      if(msg.update.gradient){
        setGradient(msg.update.gradient);
      }
    }
  };
  const staff_ref = useRef(staff);
  staff_ref.current = staff;

  const total_cols = staff.length + 1;
  const col_width = `${(100/total_cols)}%`;

  const isColumnHighlighted = (idc: number) => {
    const state = state_ref.current;
    const playing = state.playing;
    if(!playing){ return false; }
    if(state.idx === idc){ return true; }
    return false;
  }

  const updatePlayhead = (ph: number | null) => {
    // update the playhead
    if(socket){
      const update: Message = {
        timestamp: 'todo timestamp',
        id: {
          name: 'client',
        },
        auth_key: truth,
        update: {
          playhead: ph,
        }
      }
      socket.send(messageToString(update));
    }
  }

  const playColumn = (idx: number) => {
    updatePlayhead(idx);
    audioPlayColumn(staff_ref.current[idx].map(e => ((e.note) ? e.note : 0)));
  }
  
  const advancePlaying = () => {
    setState(prev => {
      let update = {...prev};
      const last = prev.idx;

      update.idx = last + 1;
      if(update.idx >= staff_ref.current.length){
        update.idx = 0;
      }

      playColumn(update.idx);
      clearTimeout(timeout);
      timeout = setTimeout(advancePlaying, period);

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
      playColumn(state_ref.current.idx);
      timeout = setTimeout(advancePlaying, period);
    }else{
      updatePlayhead(null);
    }
  }

  const showExampleSong = (id: ExampleID) => {
    // console.log('showing example song: ', id);
    if(authorized_ref.current){
      // send updated colors to sensor columns
      let msg: Message = {
        timestamp: 'todo: timestamp',
        auth_key: truth,
        id: {
          name: 'client'
        },
        update: {
          example: (Number(id) - 1),
        }
      }
      socket.send(messageToString(msg));
    }else{
      console.warn('not authorized')
    }
  }

  useEffect(() => {
    const handleKeydown = (e: any) => {

      switch(e.key){
        case ' ':
          if(!debounce.toggle_playing){
            debounce.toggle_playing = true;
            setTimeout(() => {
              debounce.toggle_playing = false;
            }, 500);
            togglePlaying();
          } else{
            console.warn('debouncing!');
          }
          break;
        
        // fall through for example numbers
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
          if(!debounce.examples){
            debounce.examples = true;
            setTimeout(() => {
              debounce.examples = false;              
            }, 500);
            showExampleSong(e.key);
          }
          break;

        default:
          console.warn('unrecognized key press');
          break;
      }
    } 

    const handleExampleRequest = (e: any) => {
      showExampleSong(e.detail);
    }

    const handleGradientChange = (e: any) => {
      const msg: Message = {
        timestamp: 'todo timestamp',
        id: {
          name: 'client',
        },
        auth_key: truth,
        update: {
          gradient: e.detail,
        }
      }
      socket.send(messageToString(msg));
    }

    window.addEventListener('keydown', handleKeydown);
    window.addEventListener('example-request', handleExampleRequest);
    window.addEventListener('gradient-change', handleGradientChange);
    
    return () => {
      // cleanup function
      window.removeEventListener('keydown', handleKeydown);
      window.removeEventListener('example-request', handleExampleRequest);
      window.removeEventListener('gradient-change', handleGradientChange);
    }
  
  }, [])
  
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

        const color = (display === 'colors') ? `#${entry.color}` : note_color_map[(entry.note) ? entry.note : 0];

        return <Box key={`staff.column.${idc}.row.${idr}`} className={classes.note} style={{backgroundColor: color}}>
        </Box> }).reverse()}
      </Box> })}
    </Box>

    {/* progress indicator */}
    <Box>
      <Box className={classes.staff}>
        <Box className={classes.column} style={{width: col_width, height: col_width}}></Box>
      {staff.map((column, idc) => {
        const highlighted = isColumnHighlighted(idc);
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

      { authorized && <>
        <GradientEditor/>
      </>}

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
    { authorized && <>
      <Picker />
    </>}
  </>}

  </>
}

export default Staff;
