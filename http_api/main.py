#NAME:  main.py
#DATE:  Wednesday 5th June 2019
#AUTH:  Ryan McCartney, EEE Undergraduate, Queen's University Belfast
#DESC:  A python script for running a cherrpi API as a serial passthrough
#COPY:  Copyright 2018, All Rights Reserved, Ryan McCartney

import threading
import cherrypy
import serial
import time
import os

try:
    
    class API(object):

        @cherrypy.expose
        def index(self):
            
            #Open serial connection when API instance created
            self.connected = False
            self.connect()

            with open ("http_api/index.html", "r") as webPage:
                contents=webPage.readlines()
            return contents

        @cherrypy.expose
        def clearLogs(self):

            #Clear Transmit Log
            log = open("http_api/public/transmitLog.csv","w")
            log.write("Date and Time,Command String Passed\n")
            log.close()

            #Clear Receive Log
            log = open("http_api/public/receiveLog.csv","w")
            log.write("Date and Time,Arm Response\n")
            log.close()

            #Return Message
            status = "INFO: Transmit and Receive Logs have been cleared."
            print(status)

            return status

        @cherrypy.expose
        def send(self,command="this"):
            
            #Get Current Date and Time for Logging
            currentDateTime = time.strftime("%d/%m/%Y %H:%M:%S")
            
            if(self.connected == False):
                status = self.connect()
    
            try:
                #Add command to transmit log
                with open ("http_api/public/transmitLog.csv", "a+") as log:
                    log.write(currentDateTime+","+command+"\n")

                #Write Command Passed to Serial Port
                payload = (str(command)+"\r\n").encode()
                self.leftArm.write(payload)

                #Read Response if Avalible
                response = ""
                while self.leftArm.in_waiting:
                    response = self.leftArm.readline()
                    
                #Add response to receive log
                with open ("http_api/public/receiveLog.html", "a+") as log:
                    log.write(currentDateTime+","+response)

                status = currentDateTime + " INFO: '"+command+"' sent succesfully."

            except:
                status = currentDateTime + " INFO: Could not send data to serial port. Check connection."
                self.connected = False

            return status

        @cherrypy.expose
        def connect(self):
            
            self.connected = False
            
            try:
                #Open Serial Connection
                self.leftArm = serial.Serial(
                    port='\\.\COM7',
                    baudrate=115200,
                    parity=serial.PARITY_NONE,
                    stopbits=serial.STOPBITS_ONE,
                    bytesize=serial.EIGHTBITS
                    )
                self.connected = True
                status = "INFO: Left arm arduino connected to "+self.leftArm.name+"\n"
            except:
                status = "ERROR: No Connection to Arduino\n"
      
            print(status)

            return status   

    if __name__ == '__main__':

        cherrypy.config.update(
            {'server.socket_host': '0.0.0.0'}
        )     
        cherrypy.quickstart(API(), '/',
            {
                'favicon.ico':
                {
                    'tools.staticfile.on': True,
                    'tools.staticfile.filename': os.path.join(os.getcwd(),'http_api\public\favicon.ico')
                },
                '/public': {
                    'tools.staticdir.on'    : True,
                    'tools.staticdir.dir'   : os.path.join(os.getcwd(),'http_api\public'),
                    'tools.staticdir.index' : 'index.html',
                    'tools.gzip.on'         : True
                }
            }
        )        
except:
    print("ERROR: Main sequence error.")
