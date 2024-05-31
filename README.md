# Project Build Instructions

## This code interacts with [NODE-RED](https://github.com/node-red/node-red)

INSTRUCTIONS (Thanks to [Raj](https://github.com/RajkumarGara) for these build instructions)

install node-red on a raspberry pi or any other Debian-based server (ex. Ubuntu server)
I've used both and both work perfect.

```shell
bash <(curl -sL https://raw.githubusercontent.com/node-red/linux-installers/master/deb/update-nodejs-and-nodered)
```

Then in order to use the dashboard do

```shell
cd ~/.node-red
npm i node-red-dashboard
node-red-restart
sudo systemctl enable nodered.service
```

Then do this in order to have access to the TPlink modules thanks to [mbserran](https://github.com/mbserran/node-red-contrib-tapo-new-api)

```shell
npm install node-red-contrib-tapo-new-api
```

## Mosquito install

You'll also need to set up a mosquito broker. This is how the Pico device will communicate to the node-red server. It also gives room to have multiple occupancy counting devices.
\
On your server do

```shell
sudo apt-get update
sudo apt-get install -y mosquitto mosquitto-clients
sudo systemctl start mosquitto
sudo systemctl enable mosquitto
```

Make sure it's installed by doing

```shell
sudo systemctl status mosquitto
```

## Pico code compile

You'll need a Pico W and the Pico SDK installed. You can find more detailed instructions on the [Raspberry Pi Pico Repo](https://github.com/raspberrypi/pico-sdk)
\
Note: Make sure you export the PICO_SDK_PATH in your kernel profile file in order to build the ultrasonic.c code.

Before you build, Make sure you set the Wi-Fi information and MQTT information.
\
On your computer do

```shell
cd path/to/project/root
mkdir build
cd build
cmake -DPICO_BOARD=pico_w ..
make
```

Press and hold the bootsel button on the Pico W while plugging it into the computer. Then, drag and drop the MainApp.uf2 file onto the Pico W.
\
For more in-dept instructions reference [Pico Docs](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)
