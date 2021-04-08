# Rainbow Forest

Rainbow Forest is an interactive experience to compose short songs on a larger-than-life musical staff. Designed for the Museum of Boulder's 2020 Plazeum 2.0 exhibit this installation engages creativity and interaction through the combination of sounds and sights.

## Firmware

There are several microcontrollers working in concert (pun intended) to make the Rainbow Forest function. They are:

- **conductor** : the main processor conducting audio operations and the overall system operation
- **network hub** : the network hub handles communication between the column controllers and the conductor 
- **column controllers** : each column of notes on the staff is managed by an individual controller

The programs that run on each of these devices can be found in ```firmware/main```

Components that are common between programs can be found in ```firmware/components```. These components are expected to be symbolically linked into the main program directories. Arduino requires additional source code to be placed within a ```src``` directory. From the repository root directory one may run the link scripts to quickly make symbolic links from every component to every main program. 

- **linux**: ```./scripts/link.sh```
- **windows**: ```scripts\link.bat```

You may also create the symbolic links manually (Linux / Windows examples follow).

- **linux**: ```ln -s firmware/components/${component} firmware/main/${program}/src/components```
- **windows**: ```mklink /J firmware\main\%program%\src\components\%component% firmware\components\%component%```

Partial / experimental programs may be contained in ```firmware/prototyping``` for reference

## Hardware

The hardware directory contains the Eagle design files and Gerber manufacturing files for the original Rainbow Forest installation.

## Web

The Rainbow Forest installation connects to the world wide web, allowing users to check in on its status from anywhere.

In the museum a single-board computer running Chromium is used as a dedicated user interface.

```
./Chromium.app/Contents/MacOS/Chromium --app=https://oclyke.dev/rainbow-forest
```

## raspberry pi
the rainbow forest uses a local raspberry pi microcomputer to provide a user interface for museum visitors. this is acheived by using Chromium in kiosk mode. the raspberry pi also translates ```ws://``` messages into ```wss://``` messages for transfer to the main server (since the esp32s were unhappy running ```wss://```) 

[headless raspberry pi configuration](https://desertbot.io/blog/headless-raspberry-pi-3-bplus-ssh-wifi-setup)
[configuring rapberry pi for kiosk mode](https://die-antwort.eu/techblog/2017-12-setup-raspberry-pi-for-kiosk-mode/)
[using systemd on the pi](https://www.raspberrypi.org/documentation/linux/usage/systemd.md)
[using node and pm2 to daemonize applications](https://dev.to/bogdaaamn/run-your-nodejs-application-on-a-headless-raspberry-pi-4jnn)
[distributions of node for raspberry pi](https://github.com/nodesource/distributions)


## License

This project is subject to the license listed in file ```LICENSE.md```