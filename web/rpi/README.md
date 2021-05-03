# rpi
a raspberry pi is used for the following purposes:
* provide visitors a dedicated way to access the web interface
* convert and relay ```ws://``` messages to ```wss://``` as they pass to the api server

## building the image (one time configuration of the rpi)

install these programs:
* git
* node
* pm2

### configure precedence for the wlan0 interface
```
sudo nano /etc/dhcpcd.conf
```
add
```
# configure priority for wlan0 interface
# lower metrics are priority, eth0 has default metric of 202
interface wlan0
metric 200
```
reboot for changes to take effect

### upgrade distros
```
sudo apt update
sudo apt dist-upgrade
```

### git
```
sudo apt install git
```

### node
the [nodesource/distributions](https://github.com/nodesource/distributions) git repo hosts shell scripts to install node. the ```deb``` directory is what you're looking for. use the script for the version you want to install. for example to install ```14.x``` do:
```
curl -sL https://raw.githubusercontent.com/nodesource/distributions/master/deb/setup_14.x | sudo -E bash -
```

### yarn 
you need yarn to get the application up and running
```
sudo npm install --global yarn
```

### pm2
sudo is needed to install this as a globally accessible script - installing without sudo will work but the pm2 binary won't be available on PATH
```
sudo yarn global add pm2
```

### clone repo
```
git clone https://github.com/oclyke-dev/RainbowForest
```

### build apps
```
cd RainbowForest/web/common
yarn build
```
and
```
cd RainbowForest/web/common
yarn build
```

### start pm2 with app
sudo is important here too because it give daemons root access
```
sudo pm2 start ~/.../RainbowForest/web/rpi/dist/rpi/src/index.js --name rainbow-forest-bridge
sudo pm2 startup
sudo pm2 save
```

## tips

remember to sudo yarn start!
```
cd rpi
sudo yarn start
 ```
