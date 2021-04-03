/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import {
  useState,
} from 'react';

import Sluicebox from './components/Sluicebox';
import Staff, { StaffType } from './components/Staff';

import {
  Box,
  Divider,
  Typography,
  Link,
} from '@material-ui/core';
import {
  createMuiTheme,
  ThemeProvider,
  makeStyles,
} from '@material-ui/core/styles';

import {
  playColumn,
} from './audio';

const useStyles = makeStyles((theme) => ({
  root: {},
  section: {
    margin: theme.spacing(2),
  },
  bullet: {
    display: 'inline-block',
    margin: '0 2px',
    transform: 'scale(0.8)',
  },
}));

// server address
const host = 'oclyke.dev';
const port = 443;
const protocol = 'wss';

// const host = '10.0.0.29';
// const port = 81;
// const protocol = 'ws';

// root of api
const api = '/rainbow-forest/api/v1';

// endpoints
const public_endpoint = `/public`;   // public namespace

// construct URL for connection
const makeWebSocketURL = (endpoint: string) => {
  return `${protocol}://${host}:${port}${api}${endpoint}`
}

// our interface to the server
const socket = new WebSocket(makeWebSocketURL(public_endpoint));

socket.onopen = () => {
  console.log('socket opened');
  
  const msg = {
    id: {
      name: 'client'
    },
    timestamp: now(),
    request: {
      staff: []
    }
  }

  socket.send(JSON.stringify(msg))
}
socket.onerror = (e) => {
  console.error(e);
};

const now = () => {
  return 'todo timestamp - client';
}

function App() {
  const classes = useStyles();

  const [staff, setStaff] = useState<StaffType>([]);
  socket.onmessage = (e) => {
    const msg = JSON.parse(e.data) as any;
    // console.log(msg.update.staff)
    if(msg.update.staff){
      setStaff(msg.update.staff);
    }
  };

  const bull = <span className={classes.bullet}>•</span>;

  return <>
    <Box display='flex' flexDirection='column' justifyContent='space-between' style={{width: '100%', height: '100%'}}>

      {/* header */}
      <Box>
        <Sluicebox>
          <Box className={classes.section} display='flex' flexDirection='row' justifyContent='space-between' >
            <Box>
              <Typography>
                Rainbow Forest
              </Typography>
            </Box>
            <Box>
              <Typography>
                museum of boulder
              </Typography>
            </Box>
          </Box>

          <Divider />
        </Sluicebox>
      </Box>

      <Box>
        <Sluicebox>
          <Box className={classes.section}>
            <Staff staff={staff}/>
          </Box>
          <Divider />
          <Box className={classes.section}>
            Show Example Song
          </Box>
          <Divider />
          <Box className={classes.section}>
            Set Light Colors
          </Box>
        </Sluicebox>
      </Box>


      {/* how-to */}
      <Box flexGrow={1} style={{overflow: 'auto'}}>
        <Sluicebox>
        <Divider />
          <Typography color='textSecondary' style={{fontSize: 22, marginTop: '8px'}}>
            how to use
          </Typography>

          {/* play a song */}
          <Typography color='textSecondary' style={{marginBottom: '0px', paddingBottom: '0px'}}>
            <i>play a song</i>
          </Typography>
          <Typography component={'span'}>
            <ul style={{marginTop: 0, listStyle: 'none'}}>
              <li>
                <span>press the play button to start playing the notes on the rainbow forest</span>
              </li>
            </ul>
          </Typography>

          {/* see an example */}
          <Typography color='textSecondary' style={{marginBottom: '0px', paddingBottom: '0px'}}>
            <i>see an example</i>
          </Typography>
          <Typography component={'span'}>
            <ul style={{marginTop: 0, listStyle: 'none'}}>
              <li>
                <span>choose an example song above to light up the forest with hints</span>
              </li>
              <li>
                <span>put the colored notes on the same colored hooks</span>
              </li>
            </ul>
          </Typography>
        </Sluicebox>
      </Box>


      <Box>
        <Sluicebox>
          <Divider style={{marginTop: '8px'}}/>
          <Typography variant='subtitle2' align='center' style={{paddingBottom: '8px', paddingTop: '8px'}}>
            <Link href='https://oclyke.dev' target='_blank' rel='noreferrer'>
              oclyke
            </Link>
            {bull}
            <Link href='https://github.com/oclyke-dev/rainbow-forest' target='_blank' rel='noreferrer'>
              GitHub
            </Link>
          </Typography>
        </Sluicebox> 
      </Box>

    </Box>
  </>
}

export default App;
