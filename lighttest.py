from PyP100 import PyL530

ip_address = "YOURIPADRESS"
light_info = {
    "username": "YOUREMAIL",
    "password": "YOURPASSWORD"
}

l530 = PyL530.L530(ip_address, light_info["username"], light_info["password"])

l530.turnOff()

l530.setBrightness(100)  # Sets the brightness of the connected bulb to 50% brightness
l530.setColorTemp(2700)  # Sets the color temperature of the connected bulb to 2700 Kelvin (Warm White)a
