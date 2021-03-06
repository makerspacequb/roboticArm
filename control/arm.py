#NAME: arm.py
#DATE: 14/06/2019
#AUTH: Ryan McCartney
#DESC: A python class for moving an entity in real-time via and http API
#COPY: Copyright 2019, All Rights Reserved, Ryan McCartney

import threading
import time
import json
import requests
import random
from requests import Session
from kinematics import Kinematic

#define threading wrapper
def threaded(fn):
    def wrapper(*args, **kwargs):
        thread = threading.Thread(target=fn, args=args, kwargs=kwargs)
        thread.start()
        return thread
    return wrapper
    
class Arm:

    debug = False
    logFilePath = "logs/log.txt"
    header = {'User-Agent':'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.101 Safari/537.36'}
    
    #Config Variable Initialise
    jointMaxRotation = []
    jointMaxSpeed = []
    jointMinSpeed = []
    jointPosDefault = []
    jointSpeedDefault = []
    jointAccelDefault = []

    def __init__(self,config, ipAddress=None):

        self.joints = 6
        self.logging = True

        if ipAddress == None:
            self.ipAddress = config["ipAddress"]        
        else:
            self.ipAddress = ipAddress

        self.port = config["port"]
        self.baseURL = "http://"+str(self.ipAddress)+":"+str(self.port)+"/"
        self.error = False
        self.timeout = 2 #Seconds
        self.pollingStatus = False

        #Values loaded from 'config.json'
        for joint in config["joints"]:
            self.jointMaxRotation.append(joint["maxRotation"])
            self.jointMaxSpeed.append(joint["maxSpeed"])
            self.jointMinSpeed.append(joint["minSpeed"])
            self.jointPosDefault.append(joint["defaultPosition"])
            self.jointSpeedDefault.append(joint["defaultSpeed"])
            self.jointAccelDefault.append(joint["defaultAccel"])

        #Status Flags
        self.jointPosition = [None]*self.joints
        self.switchState = [0]*self.joints
        self.calibrationState = [0]*self.joints
        self.movementFlag = [0]*self.joints

        try:
            self.session = requests.session()
            self.clearLogs()
            self.connected = True
        except:
            self.log("ERROR: Cannot create a session.")
            self.connected = False

        #Open a solver for kinematics
        self.kin = Kinematic()
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
    def sendCommand(self, command):
        
        #Start Timing
        start = time.time()
        #combine with host address
        message = self.baseURL + "send?command=" + command
        message = message.encode('ascii')

        if self.pollingStatus == False:
            self.getStatus()

        try:
            if self.debug == True:
                response = self.session.get(message,timeout=self.timeout)
                status = response.content.decode("utf-8").split("\n")
                self.log("INFO: Transmission response code is "+str(response.status_code))
                end = time.time()
                self.log("STATUS: Sending '"+str(command)+"' took "+str(round((end-start),2))+" seconds.")
                self.log(status[0])
            else:
                #t1 = time.time()
                self.session.get(message,timeout=self.timeout)
                #t2 = time.time()
                #print("in class ", t2 - t1)
            self.connected = True

        
        except:
            self.log("ERROR: Could not access API.")
            self.connected = False

    @threaded
    def getStatus(self):
          
        while self.connected:
            self.pollingStatus = True
            try:
                message = self.baseURL + "getLatest"
                response = self.session.get(message,timeout=self.timeout)
                status = str(response.text) 
                #Extract Joint Positions
                if(status.find("STATUS:")!=-1):
                    if(status.find("MOVEMENT") != -1):
                        data = status.split(",")
                        self.movementFlag = list(map(int,data[1:]))
                    elif(status.find("CALIBRATION") != -1):
                        data = status.split(",")
                        self.calibrationState = list(map(int,data[1:]))
                    elif(status.find("POSITION") != -1):
                        data = status.split(",")
                        try:
                            self.jointPosition = list(map(float,data[1:]))
                        except:
                            pass
                    elif(status.find("SWITCH") != -1):
                        data = status.split(",")
                        self.switchState = list(map(int,data[1:]))
                    else:
                        self.log("FAILED TO PARSE: "+status)
                elif(status !=""):
                    self.log(status)

            except:     
                self.log("INFO: Did not receive status response from API.")
        
        self.pollingStatus = False

    def moveJointTo(self,joint,position):
        if self.calibrationState[joint]:
            if (position >= 0) and (position <= self.jointMaxRotation[joint]):
                command = "p"+str(joint)+str(position)
                self.sendCommand(command)
                self.log("INFO: Joint "+str(joint)+" moved to "+str(position)+" degrees.")
            else:
                self.log("ERROR: Positon out of range.")
        else:
            self.log("ERROR: Joint "+str(joint)+" not calibrated.")

    def moveJoint(self,motor,degrees):      
        #Check movement is within range allowed
        if (int(self.jointPosition[motor])+degrees) > self.jointMaxRotation[motor]:
            degrees = self.jointMaxRotation[motor] - int(self.jointPosition[motor])
        if (int(self.jointPosition[motor])+degrees) < 0:
            degrees = -self.jointMaxRotation[motor]
        command = "m"+str(motor)+str(degrees)
        self.sendCommand(command)
        self.log("INFO: Command sent to adjust motor "+str(motor)+" "+str(degrees)+" degrees.")

    def getPose(self):
        pose = self.kin.forwardKinematics(self.jointPosition)
        return pose

    def getPositions(self):
        return self.jointPosition 

    def getJointPosition(self,motor):
        position = float(self.jointPosition[motor])
        return position 

    def positionJoints(self,positions):
        
        if self.armCalibrated():
            if len(positions) == self.joints:
                motor = 0

                for position in positions:
                    self.moveJointTo(motor,position)
                    motor += 1
            else:
                self.log("ERROR: Invalid Joint Positions.")
        else:
            self.log("ERROR: Calibrate arm before continuing.")

    def rest(self):
        if self.armCalibrated():
            self.log("INFO: Arm moving to a resting position.")
            restPosition = [None]*self.joints
            restPosition[0] = self.jointPosDefault[0]
            restPosition[1] = 150
            restPosition[2] = 175
            restPosition[3] = self.jointPosDefault[3]
            restPosition[4] = self.jointPosDefault[4]
            restPosition[5] = self.jointPosDefault[5]
            self.positionJoints(restPosition)
        else:
            self.log("ERROR: Calibrate arm before trying to rest.")

    def standUp(self):
        if self.armCalibrated():
            self.log("INFO: Arm standing upright.")
            self.positionJoints(self.jointPosDefault)
        else:
            self.log("ERROR: Calibrate arm before trying to stand.")

    def setAccel(self,joint,accel):
        command = "z"+str(joint)+str(int(accel))
        self.sendCommand(command)
        self.log("INFO: Joint "+str(joint)+" acceleration rate adjusted to "+str(int(accel))+" degrees per second squared.")
    
    def setSpeed(self,joint,speed):
        command = "s"+str(joint)+str(int(speed))
        self.sendCommand(command)
        self.log("INFO: Joint "+str(joint)+" speed adjusted to "+str(int(speed))+" degrees per second.")
    
    def setMinSpeed(self,joint,minSpeed):
        command = "d"+str(joint)+str(int(minSpeed))
        self.sendCommand(command)
        self.log("INFO: Joint "+str(joint)+" minimum speed adjusted to "+str(int(minSpeed))+" degrees per second.")
     
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
    
    def selectRandomPosition(self,motor):
        randomPosition = random.randint(0,self.jointMaxRotation[motor])
        return randomPosition

    def waitToStationary(self):
        time.sleep(0.2)
        while(self.checkMovement()):
            pass
            
    def checkMovement(self):
        time.sleep(0.2)
        moving = False
        for jointMoving in self.movementFlag:
            if bool(jointMoving):
                moving = jointMoving
        return moving

    def clearLogs(self):
        url = self.baseURL + "clearLogs"
        response = self.session.get(url,timeout=self.timeout)
        if response.content.decode("utf-8"):
            self.log(response.content.decode("utf-8"))

    def resetArduino(self):
        messages = ["disconnect","connect"]
        for message in messages:
            url = self.baseURL + message
            response = self.session.get(url,timeout=self.timeout)
            if response.content.decode("utf-8"):
                self.log(response.content.decode("utf-8"))
        time.sleep(1.5)
        self.log("INFO: Arduino for Arm Reset.")

    def resetEStop(self):
        self.sendCommand("r")
        time.sleep(1)
        self.log("INFO: Emergency Stop Latch Reset.")
        
    def stopDemos(self):
        url = self.baseURL + "stopDemos"
        response = self.session.get(url,timeout=self.timeout)
        if response.content.decode("utf-8"):
            self.log(response.content.decode("utf-8"))
        time.sleep(1.5)
        self.log("INFO: Raspberry Pi Demo Processes Terminated.")

    def armCalibrated(self):
        calibrated = True
        for jointCalibrated in self.calibrationState:
            calibrated &= int(jointCalibrated)
        return calibrated
    
    def setDefaults(self):
        i = 0 
        for i in range(0,self.joints):
            self.setSpeed(i,self.jointSpeedDefault[i])
            self.setMinSpeed(i,(int(self.jointSpeedDefault[i])-10))      
            self.setAccel(i,self.jointAccelDefault[i])
            self.log("INFO: Joint "+str(i)+" defaults set.")
