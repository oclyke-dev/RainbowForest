/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

import {
  useState,
  useEffect,
} from 'react';

import Sluicebox from './components/Sluicebox';
import Staff, { StaffType } from './components/Staff';

import {
  Box,
  Divider,
  TextField,
  InputAdornment,
  Typography,
  Link,
  Popover,
} from '@material-ui/core';
import {
  createMuiTheme,
  ThemeProvider,
  makeStyles,
} from '@material-ui/core/styles';

import {
  mob_logo
} from './assets/images';

import {
  useAuth
} from './hooks/useAuth';

const useStyles = makeStyles((theme) => ({
  root: {},
  section: {
    margin: theme.spacing(2),
  },
  auth: {
  },
  bullet: {
    display: 'inline-block',
    margin: '0 2px',
    transform: 'scale(0.8)',
  },
  mob_logo: {
    width: '150px',
    [theme.breakpoints.down('sm')]: {
      width: '100px',
    },
  }
}));

function App() {
  const classes = useStyles();

  const [auth_anchor_el, setAuthAnchorEl] = useState<HTMLButtonElement | null>(null);
  const [authorized, setAuthKey] = useAuth();

  const bull = <span className={classes.bullet}>•</span>;

  return <>
    <Box display='flex' flexDirection='column' justifyContent='space-between' style={{width: '100%', height: '100%'}}>

      {/* header */}
      <Box>
        <Sluicebox>
          <Box className={classes.section} display='flex' flexDirection='row' justifyContent='space-between' >
            <Box display='flex' flexDirection='column' justifyContent='space-around'>
              <a href='https://museumofboulder.org/'>
                <img src={mob_logo} alt='logo' className={classes.mob_logo}/>
              </a>
            </Box>
            <Box display='flex' flexDirection='column' justifyContent='space-around'>
              <Box>
                <Typography>
                  Rainbow Forest
                </Typography>
              </Box>
            </Box>
            <Box display='flex' flexDirection='column' justifyContent='space-around'>
              <Box>
                <Typography variant='subtitle2' align='center' style={{paddingBottom: '8px', paddingTop: '8px'}}>
                  <Box display='flex' flexDirection='row' justifyContent='space-around'>
                    <Box display='flex' flexDirection='row'>
                      <Link href='https://oclyke.dev' target='_blank' rel='noreferrer'>
                        oclyke
                      </Link>
                      {bull}
                      <Link href='https://github.com/oclyke-dev/rainbow-forest' target='_blank' rel='noreferrer'>
                        GitHub
                      </Link>
                    {!authorized && <>
                      {bull}
                      <Link 
                        onClick={(e: any) => {
                          setAuthAnchorEl(e.currentTarget);
                        }}
                      >
                        authorize
                      </Link>
                      <Popover
                        open={Boolean(auth_anchor_el)}
                        anchorEl={auth_anchor_el}
                        onClose={() => {
                          setAuthAnchorEl(null);
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
                        <Box className={classes.auth} display='flex' flexDirection='row' >
                          <Box display='flex' flexDirection='column' justifyContent='space-around' paddingRight={1}>
                            <Typography>
                              authorization key:
                            </Typography>
                          </Box>
                          <TextField
                            onKeyDown={(e) => {
                              if (e.key === 'Enter') {
                                setAuthKey((e as any).target.value);
                              }
                            }}
                          />
                        </Box>
                      </Popover>
                    </>}
                    </Box>
                  </Box>
                </Typography>
              </Box>
            </Box>
          </Box>

          <Divider />
        </Sluicebox>
      </Box>

      <Box>
        <Sluicebox>
          <Box className={classes.section}>
            <Staff />
          </Box>
          <Divider />
          <Box className={classes.section}>
            Show Example Song
          </Box>
        </Sluicebox>
      </Box>


      {/* how-to */}
      <Box flexGrow={1} >
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

    </Box>
  </>
}

export default App;
