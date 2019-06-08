# HTTP API

Simple API with web interface using AJAX and direct HTTP commands to allow remote control via the 'Requests' libary or 'urlib2'.

## Requirements

* `pip install cherrypy`
* `pip install pyserial`
* `pip install glob`

## Usage

1. Clone Repository to any directory on your Raspberry Pi.
2. sudo python directories/roboticArm/http_api/main.py
3. Navigate to `http://piIPaddress:8080`

## Commands to use with Requests Libary

* Send data to robotic arm as follows `http://piIPaddress:8080/send?command=ARDUINO_COMMAND_HERE`
* Clear Logs files on Pi `http://piIPaddress:8080/clearLogs`
* Acquire Receive log `http://piIPaddress:8080/public/receiveLog.csv`
* Acquire Transmit log `http://piIPaddress:8080/public/transmitLog.csv`
* Connect or Reconnect Serial `http://piIPaddress:8080/connect`

Note: The serial port is automatically connected when using the `send?command=ARDUINO_COMMAND_HERE` function. The connect command is best used to manage disconnect errors. 