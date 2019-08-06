#NAME: mapping.py
#DATE: 14/06/2019
#AUTH: Ryan McCartney
#DESC: Python function to control a 6DOF robotic arm with an xbox controller.
#COPY: Copyright 2019, All Rights Reserved, Ryan McCartney

import sys
sys.path.append('control/')
from arm import Arm
import pygame
import time
import json

class Controller:

    debug = False
    logFilePath = "logs/log.txt"
    deadzone = 0.3
    #COPY: Copyright 2019, All Rights Reserved, Ryan McCartney

    def __init__(self,config):

        self.logging = True
        self.gamepadConnected = False
        self.gamepadToUse = 0

        #Create instance of Arm class
        self.arm = Arm(config)
        self.arm.reset()

        #Start Pygame
        pygame.init()
        pygame.joystick.init()

        #Setup Variables for Buttons
        self.buttonTotal = 10
        self.lastButtonState = [0]*self.buttonTotal

        #Setup Variables for Axis
        self.axisTotal = 5
        self.leftJoint = 0
        self.rightJoint = 1
        self.axisPositions = [0]*self.axisTotal
        self.setSpeed = [0]*self.arm.joints
    
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
    
    def mapJoysticks(self):
        status = True
        try:
            #Capture Button States
            for i in range(0,self.axisTotal):
                self.axisPositions[i] = self.gamepad.get_axis(i)
            self.mapJoystick(self.axisPositions[0],self.axisPositions[1],self.leftJoint)
            self.mapJoystick(self.axisPositions[3],self.axisPositions[4],self.rightJoint)

        except:
            self.log("ERROR: Mapping Axis Error.")
            status = False
        return status

    def mapJoystick(self,xAxis,yAxis,joint):
        joint = joint+joint
        self.mapJoint(joint,xAxis)
        self.mapJoint(joint+1,yAxis)

    def mapButtons(self):
        buttonState = [0]*self.buttonTotal
        status = True
        try:
            #Capture Button States
            for i in range(0,self.buttonTotal):
                buttonState[i] = self.gamepad.get_button(i)

            #A BUTTON - STOP
            if(buttonState[0] and (self.lastButtonState[0] == 0)):
                self.arm.reset()
            #B BUTTON - STOP
            if(buttonState[1] and (self.lastButtonState[1] == 0)):
                self.arm.stop()
            #Y BUTTON - STANDUP
            if(buttonState[3] and (self.lastButtonState[3] == 0)):
                self.arm.setDefaults()
                self.arm.standUp()
                self.arm.waitToStationary()
            #X BUTTON - REST
            if(buttonState[2] and (self.lastButtonState[2] == 0)):
                self.arm.setDefaults()
                self.arm.rest()
                self.arm.waitToStationary()
            #START BUTTON - CALIBRATE ARM
            if(buttonState[7] and (self.lastButtonState[7] == 0)):
                self.arm.setDefaults()
                self.arm.calibrateArm()
                self.arm.waitToStationary()
            #LEFT THUMB BUTTON - CHANGE JOINT
            if(buttonState[8] and (self.lastButtonState[8] == 0)):
                if (self.leftJoint + 1) == self.rightJoint:
                    self.leftJoint += 2
                elif (self.leftJoint) > 2:
                    self.leftJoint = 0
                else:
                    self.leftJoint += 1
                self.log("INFO: Joint set "+str(self.leftJoint)+" selected on left joystick.")
            #RIGHT THUMB BUTTON - CHANGE JOINT
            if(buttonState[9] and (self.lastButtonState[9] == 0)):  
                if (self.rightJoint + 1) == self.leftJoint:
                    self.rightJoint += 2
                if (self.rightJoint + 1) > 2:
                    self.rightJoint = 0
                else:
                    self.rightJoint += 1
                self.log("INFO: Joint set "+str(self.rightJoint)+" selected on right joystick.")
            
            self.lastButtonState = buttonState
        except:
            self.log("ERROR: Mapping Buttons Error.")
            status = False
        return status
    
    def gamepads(self):
        gamepads = pygame.joystick.get_count()
        self.log("INFO: There are "+str(gamepads)+" connected to the PC.")
        return gamepads
    
    def getGamepadData(self):
        status = True
        try:
            #Get Current Data
            pygame.event.get()
        except:
            status = False
        return status

    def connectGamepad(self):
        #Initialise first gamepad
        self.gamepad = pygame.joystick.Joystick(self.gamepadToUse)
        self.gamepad.init()
        self.log("INFO: Gamepad Connected Succesfully.")
    
    def mapJoint(self,joint,rawPosition):
        
        #Converting to Discrete Speed Control
        rawPosition = round(rawPosition,2)

        minSpeed = self.arm.jointMinSpeed[joint]
        maxSpeed = self.arm.jointMaxSpeed[joint]
        maxRotation = abs(self.arm.jointMaxRotation[joint])

        #Select Direction
        if rawPosition > self.deadzone:
            #Select and Set a Speed
            mappedSpeed = self.mapToRange(abs(rawPosition),self.deadzone,1,minSpeed,maxSpeed)
            if mappedSpeed != self.setSpeed[joint]:
                self.arm.setMinSpeed(joint,mappedSpeed)
                self.arm.setSpeed(joint,mappedSpeed)
                self.setSpeed[joint] = mappedSpeed
                #Move the arm
                self.arm.moveJointTo(joint,maxRotation)
        elif rawPosition < -self.deadzone:
            #Select and Set a Speed
            mappedSpeed = self.mapToRange(abs(rawPosition),self.deadzone,1,minSpeed,maxSpeed)
            if mappedSpeed != self.setSpeed[joint]:
                self.arm.setMinSpeed(joint,mappedSpeed)
                self.arm.setSpeed(joint,mappedSpeed)
                #Move the arm
                self.setSpeed[joint] = mappedSpeed
                self.arm.moveJointTo(joint,0)
        else:
            if self.arm.armCalibrated():
                if self.setSpeed[joint] != 0:
                    self.arm.setMinSpeed(joint,0)
                    self.arm.setSpeed(joint,0)
                    self.setSpeed[joint] = 0
    
    @staticmethod
    def mapToRange(raw,rawMin,rawMax,mapMin,mapMax):
        mapped = (raw - rawMin) * (mapMax - mapMin) / (rawMax - rawMin) + mapMin
        return mapped