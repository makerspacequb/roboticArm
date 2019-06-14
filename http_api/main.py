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

SerialPort = '/dev/ttyACM0'
Baudrate = 115200
SerialMonitorLines = 20

#define threading wrapper
def threaded(fn):
    def wrapper(*args, **kwargs):
        thread = threading.Thread(target=fn, args=args, kwargs=kwargs)
        thread.start()
        return thread
    return wrapper

try:
    
    class API(object):

        connected = False
        serialMonitorData = ["-,-"]*SerialMonitorLines

        @cherrypy.expose
        def index(self):
            
            #On index try to connect to serial
            self.connect()

            with open ("http_api/index.html", "r") as webPage:
                contents=webPage.readlines()
            return contents

        @cherrypy.expose
        def clearLogs(self):

            currentDateTime = time.strftime("%d/%m/%Y %H:%M:%S")

            #Clear Transmit Log
            log = open("http_api/public/transmitLog.csv","w")
            log.write("Date and Time,Command String Passed\n")
            log.close()

            #Clear Receive Log
            log = open("http_api/public/receiveLog.csv","w")
            log.write("Date and Time,Robotic Arm Response\n")
            log.close()
            
            #Clear serial monitor
            self.serialMonitorData = ["-,-"]*SerialMonitorLines

            #Return Message
            status = currentDateTime + " - INFO: Transmit and Receive Logs have been cleared."
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
                self.serial.reset_output_buffer()
                payload = (str(command)+"\n").encode()
                self.serial.write(payload)
                
                status = currentDateTime + " - INFO: '" + command + "' sent succesfully."

            except:
                status = currentDateTime + " - ERROR: Could not send '"+ command +"' to serial port. Check connection."
                self.connected = False

            print(status)
            return status

        @threaded
        def receive(self):
            
            #Initialise array to store data serial monitor data
            self.serialMonitorData = ["-,-"]*SerialMonitorLines

            while self.connected == True:
                
                #Get Current Date and Time for Logging
                currentDateTime = time.strftime("%d/%m/%Y %H:%M:%S")
                #Read Response if Avalible
                response = "VOID"
                
                try:
                    if self.serial.in_waiting > 0:
                        response = self.serial.readline().decode('utf-8')
                    
                        response = response.strip()
                        logLine = currentDateTime+","+str(response)

                        #Add response to receive log
                        with open ("http_api/public/receiveLog.csv", "a+") as log:
                            log.write(logLine+"\n")
                                                
                        #Add received data to serial monitor array
                        self.serialMonitorData.pop(0)
                        self.serialMonitorData.append(logLine)        

                        print(logLine)
                except:
                    self.connected = False
                    currentDateTime = time.strftime("%d/%m/%Y %H:%M:%S")
                    status = currentDateTime + " - ERROR: Cannot read serial line."
                    print(status)

        @cherrypy.expose
        def serialMonitor(self):
            
            table = "<table><tr><th>Timestamp</th><th>Serial Data</th></tr>"

            for row in self.serialMonitorData:

                table += "<tr><td width='30%'>"
                table += row.replace(",", "</td><td width='70%'>")
                table += "</td></tr>"
        
            table +="</table>"
            return table

        @cherrypy.expose
        def getLine(self):

            line = self.serialMonitorData[SerialMonitorLines-1]
            return line

        @cherrypy.expose
        def connect(self):

            currentDateTime = time.strftime("%d/%m/%Y %H:%M:%S")
            status = currentDateTime + " - INFO: Motor control box arduino already connected."

            if(self.connected == False):
                

                try:
                    #Open Serial Connection
                    self.serial = serial.Serial(
                        port= SerialPort,
                        baudrate=Baudrate,
                        parity=serial.PARITY_NONE,
                        stopbits=serial.STOPBITS_ONE,
                        bytesize=serial.EIGHTBITS,
                        timeout=0.1
                        )
                    self.connected = True
                    self.receive()
                    status = currentDateTime + " - INFO: Motor control box connected to "+self.serial.name+".\n"
                except:
                    status = currentDateTime + " - ERROR: Could not establish a connection with motor control box.\n"
      
            print(status)

            return status   

        @cherrypy.expose
        def getImage():

            image = "NOT YET OPERATIONAL"
            
            return image
    if __name__ == '__main__':

        cherrypy.config.update(
            {'server.socket_host': '0.0.0.0'}
        )     
        cherrypy.quickstart(API(), '/',
            {
                'favicon.ico':
                {
                    'tools.staticfile.on': True,
                    'tools.staticfile.filename': os.path.join(os.getcwd(),'http_api/public/favicon.ico')
                },
                '/public': {
                    'tools.staticdir.on'    : True,
                    'tools.staticdir.dir'   : os.path.join(os.getcwd(),'http_api/public'),
                    'tools.staticdir.index' : 'index.html',
                    'tools.gzip.on'         : True
                }
            }
        )        
except:
    print("ERROR: Main sequence error.")
