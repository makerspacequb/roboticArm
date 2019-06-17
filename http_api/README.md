# HTTP API

Simple API with web interface using AJAX and direct HTTP commands to allow remote control via the 'Requests' libary or 'urlib2'.

## Requirements

* `pip3 install cherrypy`
* `pip3 install pyserial`

## Installation and Run

1. `cd /home/pi/Desktop/`
2. `git clone https://github.com/makerspacequb/roboticArm`
3. `cd ~/Desktop/roboticArm/http_api`
4. `sudo chmod +x startServer.sh`
5. `./startServer.sh`

## Usage

1. Clone Repository to any directory on your Raspberry Pi.
2. sudo python directories/roboticArm/http_api/main.py
3. Navigate to `http://PI_IP_ADDRESS:8080` or `http://HOSTNAME.local:8080`

## Commands to use with Requests Libary

* Send data to robotic arm as follows `http://PI_IP_ADDRESS:8080/send?command=ARDUINO_COMMAND_HERE`
* Clear Logs files on Pi `http://PI_IP_ADDRESS:8080/clearLogs`
* Acquire Receive log `http://PI_IP_ADDRESS:8080/public/receiveLog.csv`
* Acquire Transmit log `http://PI_IP_ADDRESS:8080/public/transmitLog.csv`
* Connect or Reconnect Serial `http://PI_IP_ADDRESS:8080/connect`
* Get latest serial monitor data in table form `http://PI_IP_ADDRESS:8080/serialMonitor`
* Get latest serial monitor line as string `http://PI_IP_ADDRESS:8080/getLine`

Note: The serial port is automatically connected when using the `send?command=ARDUINO_COMMAND_HERE` function. The connect command is best used to manage disconnect errors. 
