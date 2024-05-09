from PyP100 import PyL530

ip_address = "10.3.113.67"
light_info = {
    "username": "martin.carapia@outlook.com",
    "password": "gk6qmpp9"
}

l530 = PyL530.L530("10.3.113.67", light_info["username"], light_info["password"])

l530.turnOff()

l530.setBrightness(100)  # Sets the brightness of the connected bulb to 50% brightness
l530.setColorTemp(2700)  # Sets the color temperature of the connected bulb to 2700 Kelvin (Warm White)a