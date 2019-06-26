#NAME: arm.py
#DATE: 14/06/2019
#AUTH: Ryan McCartney
#DESC: A python class for moving an entity in real-time via and http API
#COPY: Copyright 2019, All Rights Reserved, Ryan McCartney

import threading
import time
import requests
from requests import Session

#define threading wrapper
def threaded(fn):
    def wrapper(*args, **kwargs):
        thread = threading.Thread(target=fn, args=args, kwargs=kwargs)
        thread.start()
        return thread
    return wrapper
    
class Arm:

    debug = False
    logFilePath = "control/log.txt"

    def __init__(self,ipAddress):

        self.logging = True
        self.baseURL = "http://"+ipAddress+":8080/"
        self.error = False
        self.timeout = 4 #Seconds
        self.jointPosition = [0,0,0,0,0,0]
        self.jointPositionMax = [350,190,190,350,180,350]

        try:
            self.session = requests.session()
            self.connected = True
        except:
            self.log("ERROR: Cannot create a session.")
            self.connected = False
        #Start capturing status packets
        self.getStatus()

    #Logging Function
    def log(self, entry):
        
        currentDateTime = time.strftime("%d/%m/%Y %H:%M:%S")
        logEntry = currentDateTime + ": " + entry

        if self.logging == True:
            #open a txt file to use for logging
            logFile = open(self.logFilePath,"a+")
            logFile.write(logEntry+"\n")
            logFile.close()

        print(logEntry)
    
    #Send and Receive Messages with implemented logging
    @threaded
    def sendCommand(self, command):

        #Start Timing
        start = time.time()

        #combine with host address
        message = self.baseURL + "send?command=" + command
    
        try:
            response = self.session.get(message,timeout=self.timeout)
            status = response.content.decode("utf-8").split("\n")

            if self.debug == True:
                self.log("INFO = Transmission response code is "+str(response.status_code))
                end = time.time()
                print("STATUS: Sending '",command,"' took %.2f seconds." % round((end-start),2))
                self.log(status)
            self.connected = True

        except:
            self.log("ERROR = Could not access API")
            self.connected = False

    @threaded
    def getStatus(self):
        
        delay = 0.25 #Seconds
        while self.connected:

            try:
                message = self.baseURL + "getLine"
                response = self.session.get(message,timeout=self.timeout)
            
                status = response.content.decode("utf-8").split("\n")
                self.log(status)
                
                #Extract Joint Positions
                if(status.find("STATUS:")>0):
                    

            except:
                self.log("ERROR: No status response. No Connection.")
                self.connected = False
        
            time.sleep(delay)
    
    def moveTo(self,motor,position):

        if position > 0:
            command = "p"+str(motor)+str(position)
            self.sendCommand(command)

        #Log the move
        self.log("INFO: Joint "+str(motor)+" moved to "+str(degrees)+" degrees.")

    def move(self,motor,degrees):      
        #Check movement is within range allowed
        if (self.jointPosition[motor]+degrees) > self.jointPositionMax[motor]:
            degrees = self.jointPositionMax[motor] - (self.jointPosition[motor]+degrees)

        command = "m"+str(motor)+str(degrees)
        self.sendCommand(command)

        #Update joint positions
        self.jointPosition[motor] = self.jointPosition[motor]+degrees

        self.log("INFO: Joint "+str(motor)+" adjusted "+str(degrees)+" degrees.")

    def standUp(self):
        moveTo(0,180)
        moveTo(1,42)
        moveTo(2,150)
        moveTo(3,170)
        moveTo(4,90)
        moveTo(5,155)
    
    def lieDown(self):
        moveTo(0,180)
        moveTo(1,42)
        moveTo(2,150)
        moveTo(3,170)
        moveTo(4,90)
        moveTo(5,155)

    def speed(self,motor,speed):
        command = "s"+str(motor)+str(speed)
        self.sendCommand(command)
        self.log("INFO: Joint "+str(motor)+" speed adjusted to "+str(speed)+" degrees per second.")
     
    def calibrate(self):
        command = "c"
        self.sendCommand(command)
        self.log("INFO: Arm is Currently Calibrating.")

    def stop(self):
        self.sendCommand("q")
        self.log("INFO: Arm Emergency Stopped.")

    def checkConnection(self):
        self.sendCommand("test")
        self.log("INFO: Testing the connection.")
        