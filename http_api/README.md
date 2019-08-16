# HTTP API

Simple API with web interface using AJAX and direct HTTP commands to allow remote control via the 'Requests' libary or 'urlib2'.

## Requirements

* `pip3 install cherrypy`
* `pip3 install pyserial`
* `pip3 install pygame`
* `pip3 install requests`

## Installation and Run

1. `cd /home/pi/Desktop/`
2. `git clone https://github.com/makerspacequb/roboticArm`
3. `cd ~/Desktop/roboticArm/http_api`
4. `sudo python3 install.py`
5. `sudo reboot`

## Usage

1. Clone Repository to any directory on your Raspberry Pi.
2. sudo python directories/roboticArm/http_api/main.py
3. Navigate to `http://PI_IP_ADDRESS:80` or `http://HOSTNAME.local:80`

## Commands to use with Requests Libary

* Send data to robotic arm as follows `http://PI_IP_ADDRESS:80/send?command=ARDUINO_COMMAND_HERE`
* Clear Logs files on Pi `http://PI_IP_ADDRESS:80/clearLogs`
* Acquire Receive log `http://PI_IP_ADDRESS:80/public/receiveLog.csv`
* Acquire Transmit log `http://PI_IP_ADDRESS:80/public/transmitLog.csv`
* Connect or Reconnect Serial `http://PI_IP_ADDRESS:80/connect`
* Get latest serial monitor data in table form `http://PI_IP_ADDRESS:80/serialMonitor`
* Get latest serial monitor line as string `http://PI_IP_ADDRESS:80/getLine`
* Disconect the serial line from Raspberry Pi `http://PI_IP_ADDRESS:80/disconnect`

Note: The serial port is automatically connected when using the `send?command=ARDUINO_COMMAND_HERE` function. The connect command is best used to manage disconnect errors.
