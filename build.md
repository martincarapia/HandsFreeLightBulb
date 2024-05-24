# This code interacts with NODE-RED (include github node-red link here)

INSTRUCTIONS (Thanks to [Raj](https://github.com/RajkumarGara) for these build instructions)

install node-red

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
