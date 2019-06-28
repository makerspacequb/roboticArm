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
        self.pollingStatus = False
        self.jointPositionMax = [350,190,190,350,180,350]

        #Status Flags
        self.jointPosition = [0,0,0,0,0,0]
        self.switchState = [0,0,0,0,0,0]
        self.calibrationState = [0,0,0,0,0,0]
        self.movementFlag = [0,0,0,0,0,0]

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
        
        if self.pollingStatus == False:
            self.getStatus()

        try:
            if self.debug == True:
                response = self.session.get(message,timeout=self.timeout)
                status = response.content.decode("utf-8").split("\n")
                self.log("INFO = Transmission response code is "+str(response.status_code))
                end = time.time()
                print("STATUS: Sending '",command,"' took %.2f seconds." % round((end-start),2))
                self.log(status[0])
            else:
                self.session.get(message,timeout=self.timeout)
            self.connected = True
        except:
            self.log("ERROR = Could not access API")
            self.connected = False

    @threaded
    def getStatus(self):
        delay = 0.1 #Seconds
        self.pollingStatus = True

        while self.connected:

            try:
                message = self.baseURL + "getLatest"
                response = self.session.get(message,timeout=self.timeout)
                status = response.content.split("\n")
            
                #Extract Joint Positions
                if(status[0].find("STATUS:")!=-1):
                    if(status[0].find("MOVEMENT") != -1):
                        data = status[0].split(",")
                        self.movementFlag = data[1:]
                    elif(status[0].find("CALIBRATION") != -1):
                        data = status[0].split(",")
                        self.calibrationState = data[1:]
                    elif(status[0].find("POSITION") != -1):
                        data = status[0].split(",")
                        self.jointPosition = data[1:]
                    elif(status[0].find("SWITCH") != -1):
                        data = status[0].split(",")
                        self.switchState = data[1:]
                    else:
                        self.log("FAILED TO PARSE: "+status[0])
                elif(status[0] !=""):
                    self.log(status[0])

            except:     
                time.sleep(delay)
        
        self.pollingStatus = False

    def moveTo(self,motor,position):

        if position > 0:
            command = "p"+str(motor)+str(position)
            self.sendCommand(command)

        #Log the move
        self.log("INFO: Joint "+str(motor)+" moved to "+str(position)+" degrees.")

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
        self.moveTo(0,180)
        self.moveTo(1,42)
        self.moveTo(2,150)
        self.moveTo(3,170)
        self.moveTo(4,90)
        self.moveTo(5,155)
    
    def lieDown(self):
        self.moveTo(0,180)
        self.moveTo(1,150)
        self.moveTo(2,170)
        self.moveTo(3,165)
        self.moveTo(4,90)
        self.moveTo(5,155)

    def speed(self,motor,speed):
        command = "s"+str(motor)+str(speed)
        self.sendCommand(command)
        self.log("INFO: Joint "+str(motor)+" speed adjusted to "+str(speed)+" degrees per second.")
     
    def calibrateArm(self):
        command = "ca"
        self.sendCommand(command)
        self.log("INFO: Arm is Currently Calibrating.")
             
    def calibrateJoint(self, joint):
        command = "c"+joint
        self.sendCommand(command)
        self.log("INFO: Joint "+str(joint)+" is currently calibrating.")

    def stop(self):
        self.sendCommand("q")
        self.log("INFO: Arm Emergency Stopped.")

    def checkConnection(self):
        self.sendCommand("test")
        self.log("INFO: Testing the connection.")
  
    def checkMovement(self):
        moving = True
        for jointMoving in self.movementFlag:
            moving &= int(jointMoving)
        return moving

    def reset(self):

        messages = ["disconnect","connect"]

        for message in messages:
            url = self.baseURL + message
            response = self.session.get(url,timeout=self.timeout)
            if response.content.decode("utf-8"):
                self.log(response.content.decode("utf-8"))

        time.sleep(2)
        self.log("INFO: Arm Reset.")

    def armCalibrated(self):
        calibrated = True
        for jointCalibrated in self.calibrationState:
            calibrated &= int(jointCalibrated)
        if(calibrated):
            self.log("INFO: Arm is fully calibrated.")
        return calibrated