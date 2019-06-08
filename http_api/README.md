#HTTP API

Simple API with web interface using AJAX and direct HTTP commands to allow remote control via the 'Requests' libary or 'urlib2'.

##Requirements

*'pip install cherrypy'
*'pip install pyserial'
*'pip install glob'

##Usage

1. Clone Repository to any directory on your Raspberry Pi.
2. sudo python directories/roboticArm/http_api/main.py
3. Navigate to 'piIPaddress:8080'

##Commands to use with Requests Libary

* Send data to robotic arm as follows 'htttp://piIPaddress:8080/send?command=COMMANDHERE'
* Clear Logs files on Pi 'http://piIPaddress:8080/clearLogs'
* Acquire Receive log 'http://piIPaddress:8080/public/receiveLog.csv'
* Acquire Transmit log 'http://piIPaddress:8080/public/transmitLog.csv'
* Connect or Reconnect Serial 'piIPaddress:8080/connect'

Note: The serial port is automatically connected when using the 'send?=command' function. The connect command is best used to manage disconnect errors. 