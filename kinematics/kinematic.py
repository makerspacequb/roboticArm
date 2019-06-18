#NAME: kinematic.py
#DATE: 18/06/2019
#AUTH: Ryan McCartney
#DESC: A python class for AR2 foward and reverse kinematics
#COPY: Copyright 2019, All Rights Reserved, Ryan McCartney

import numpy as np
import time
import math
import threading

#define threading wrapper
def threaded(fn):
    def wrapper(*args, **kwargs):
        thread = threading.Thread(target=fn, args=args, kwargs=kwargs)
        thread.start()
        return thread
    return wrapper
    
class Kinematic:

    debug = False
    logFilePath = "kinematics/log.txt"
    
    def __init__(self,joints):

        self.logging = True
        self.joints =  joints

        #Constant matrices
        self.d = [169.77,0,0,-222.63,0,-36.25] #Distance Matrix (Arm Lengths)
        self.a = [64.2,305,0,0,0,0] #A Matrix

        #Workspace Reference Point (Where should zero?)
        self.workFrameX = 0
        self.workFrameY = 0
        self.workFrameZ = 0
        self.workFrameRoll = 0
        self.workFramePitch = 0
        self.workFrameYaw = 0

        #Tool offsets
        self.toolFrameX = 0
        self.toolFrameX = 0
        self.toolFrameX = 0
        self.toolFrameX = 0
        self.toolFrameX = 0
        self.toolFrameX = 0
    
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

    def forwardKinematic(self,jointPositions):

        #Convert joinnt positions to radians
        theta = math.radians(jointPositions)
        alpha = math.radians([-90,0,90,-90,90,0])
        
        workFrame = self.generateWorkFrame()
        toolFrame = self.generateToolFrame()

        for i in range(0,self.joints):

            self.log("INFO: Matrix Multiplication")
            
        x = 0
        y = 0
        z = 0
        roll  = 0
        pitch = 0
        yaw = 0

        return x,y,z,roll,pitch,yaw
    
    #Generate Work Frame Matrix from the X,Y,Z,Roll,Pitch,Yaw Reference
    def generateWorkFrame(self):
        
        workFrame = np.zeros(4, dtype=float)

        return workFrame
    
    #Generate Work Frame Matrix from the X,Y,Z,Roll,Pitch,Yaw Tool Information
    def generateToolFrame(self):
        
        toolFrame = np.zeros(4, dtype=float)

        return toolFrame

    def inverseKinectic(self,x,y,z,roll,pitch,yaw):

        jointPositions = [0]*self.joints

        return jointPositions