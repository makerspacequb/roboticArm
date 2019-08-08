#NAME:  main.py
#DATE:  Tuesday 6th August 2019
#AUTH:  Ryan McCartney, EEE Undergraduate, Queen's University Belfast
#DESC:  A python script for running a CherryPy API as a serial passthrough
#COPY:  Copyright 2019, All Rights Reserved, Ryan McCartney

import subprocess
import threading
import cherrypy
import signal
import serial
import time
import json
import os

cofigFilePath =  "http_api/settings.json"

#define threading wrapper
def threaded(fn):
    def wrapper(*args, **kwargs):
        thread = threading.Thread(target=fn, args=args, kwargs=kwargs)
        thread.start()
        return thread
    return wrapper

try:
    class API(object):

        def __init__(self,configFilePath):
            

            self.loadConfig(cofigFilePath)
            
            #Initiialise other Variables
            self.headers = ["Timestamp","Info","Joint 0","Joint 1","Joint 2","Joint 3","Joint 4","Joint 5"]
            self.serialMonitorData = ["-,-,-,-,-,-,-,-"]*self.serialMonitorLines
            self.connected = False
            self.latestMessage = ""
            self.previousMessage = ""
            self.indexPrepared = False
            self.processes = []

            #Update Server Port
            cherrypy.config.update(
                {'server.socket_host': '0.0.0.0',
                 'server.socket_port': self.serverPort}
            )   
     
            #On startup try to connect to serial
            self.connect()
            self.runDemo(self.xboxControlPath)
        
        def loadConfig(self,configFilePath):
            with open(configFilePath) as configFile:  
                config = json.load(configFile)
                self.serverName  = config["serverName"]
                self.serverPort  = config["serverPort"]
                self.serialPort  = config["serialPort"]
                self.baudrate  = config["baudrate"]
                self.serialMonitorLines  = config["serialMonitorLines"]
                self.hostname  = config["hostname"]
                self.xboxControlPath = config["xboxScript"]
       
        @cherrypy.expose
        def index(self):
            if not self.indexPrepared:
                self.prepareIndex()
            #On index try to connect to serial
            self.connect()

            with open ("http_api/index.html", "r") as webPage:
                contents=webPage.readlines()
            return contents
        
        def prepareIndex(self):
            contents = ""
            with open("http_api/baseIndex.html", "rt") as webPageIn:
                for line in webPageIn:
                    contents += line.replace('SERVERNAMEFEILD',self.serverName)
            with open("http_api/index.html", "wt") as webPageOut:
                    webPageOut.write(contents)
                    self.indexPrepared = True

        @cherrypy.expose
        def demos(self):
            with open ("http_api/demo.html", "r") as webPage:
                contents=webPage.readlines()
            return contents

        @cherrypy.expose
        def runDemo(self,demoPath):
            try:
                self.stopDemos()
                cwd = os.getcwd()
                fullPath = cwd+demoPath
                command = "exec python3 "+fullPath
                p = subprocess.Popen(command,stdout=subprocess.PIPE, shell=True)
                time.sleep(2)
                self.processes.append(p)
                status = "Successfully Running Demo from '"+str(demoPath)+"' with PID: "+str(p.pid)+"."
            except:
                status = "Failed to run '"+str(demoPath)+"'."
            return status

        @cherrypy.expose
        def stopDemos(self):
            status = "Successfully Terminated Demos."
            try:   
                while self.processes:
                    p = self.processes.pop()
                    p.kill()
                    time.sleep(1)
                    status = "INFO: Terminated Process "+str(p.pid)+"."
                self.disconnect()
                self.connect()
            except:
                status = "Failed to terminate demo scripts."
            return status

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
            self.serialMonitorData = ["-,-,-,-,-,-,-,-"]*self.serialMonitorLines

            #Return Message
            status = currentDateTime + " - INFO: Transmit and Receive Logs have been cleared."
            print(status)

            return status

        @cherrypy.expose
        def send(self,command="this"):
            
            #Get Current Date and Time for Logging
            currentDateTime = time.strftime("%d/%m/%Y %H:%M:%S")
            
            if not self.connected:
                status = self.connect()
            if self.processes:
                self.stopDemos()
    
            try:
                #Add command to transmit log
                with open ("http_api/public/transmitLog.csv", "a+") as log:
                    log.write(currentDateTime+","+command+"\n")

                #Write Command Passed to Serial Port
                payload = (command+'\n').encode('ascii')
                self.serial.write(payload)
                time.sleep(0.008)

                status = currentDateTime + " - INFO: '" + command + "' sent succesfully."

            except:
                status = currentDateTime + " - ERROR: Could not send '"+ command +"' to serial port. Check connection."
                self.connected = False

            print(status)
            return status

        @threaded
        def receive(self):
            
            #Initialise array to store data serial monitor data
            self.serialMonitorData = ["-,-"]*self.serialMonitorLines

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
                        self.latestMessage = response

                        #Add response to receive log
                        with open ("http_api/public/receiveLog.csv", "a+") as log:
                            log.write(logLine+"\n")
                                                
                        #Add received data to serial monitor array
                        self.serialMonitorData.pop(0)
                        self.serialMonitorData.append(logLine)        
                        #print(logLine)
                    if self.serial.in_waiting > 200:
                        self.serial.reset_input_buffer()
                        dump = self.serial.readline().decode('utf-8')
                        currentDateTime = time.strftime("%d/%m/%Y %H:%M:%S")
                        #status = currentDateTime + " - ERROR: Buffer full dumping '"+str(dump)+"'."
                        #print(status)
                except:
                    self.connected = False
                    currentDateTime = time.strftime("%d/%m/%Y %H:%M:%S")
                    status = currentDateTime + " - ERROR: Cannot read serial line."
                    print(status)

        @cherrypy.expose
        def serialMonitor(self):
            
            #Add Correct number of Headers
            table =  "<table><tr>"
            for header in self.headers:
                table += "<th>"+header+"</th>"
            table += "</tr>"

            #Get table contents
            rows = len(self.serialMonitorData)-1
            for i in range(rows,0,-1):
                row = self.serialMonitorData[i]
                table += "<tr><td width='20%'>"
                table += row.replace(",", "</td><td width='10%'>",len(self.headers))
                if row.count(',') < len(self.headers):
                    for i in range(row.count(','),len(self.headers)-1):
                        table += "</td><td width='10%'>"
                table += "</td></tr>"
    
            table +="</table>"
            return table


        @cherrypy.expose
        def getLast(self):
            return self.latestMessage

        @cherrypy.expose
        def getLatest(self):

            if self.previousMessage == self.latestMessage:
                message = "" 
            else:
                message = self.latestMessage

            self.previousMessage = self.latestMessage

            return message

        @cherrypy.expose
        def connect(self):

            currentDateTime = time.strftime("%d/%m/%Y %H:%M:%S")
            status = currentDateTime + " - INFO: Motor control box arduino already connected."

            if(self.connected == False):
                
                try:
                    #Open Serial Connection
                    self.serial = serial.Serial(
                        port= self.serialPort,
                        baudrate=self.baudrate,
                        parity=serial.PARITY_NONE,
                        stopbits=serial.STOPBITS_ONE,
                         bytesize=serial.EIGHTBITS,
                        )
                    time.sleep(1)
                    self.connected = True
                    self.receive()
                    status = "INFO: Motor control box connected to "+self.serial.name+"."
                except:
                    status = "ERROR: Could not establish a connection with motor control box."
      
            print(status)

            return status   

        @cherrypy.expose
        def disconnect(self):

            status = "INFO: Motor control box is not connected."

            if(self.connected == True):
                self.serial.close()
                self.connected = False
                status = "INFO: Motor control box disconnected."

            print(status)
            return status   

        @cherrypy.expose
        def getImage(self):

            image = "NOT YET OPERATIONAL"
            
            return image

    if __name__ == '__main__':

        cherrypy.config.update(
            {'server.socket_host': '0.0.0.0'}
        )     
        cherrypy.quickstart(API(cofigFilePath), '/',
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
