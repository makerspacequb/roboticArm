#NAME: standSit.py
#DATE: 03/07/2019
#AUTH: Ryan McCartney
#DESC: Python function to randomly move the AR2. Then return to a predefined point
#COPY: Copyright 2019, All Rights Reserved, Ryan McCartney

from arm import Arm
import time
import json

global config
ip_address = "192.168.0.105"
fixedPosition = [170,40,40,40,40,180]

#Clear Log File
open('control/log.txt', 'w').close()

#Collect Config
with open('control/config.json') as json_file:  
    config = json.load(json_file)

#Create instance of Arm class
arm = Arm(ip_address,config)
arm.reset()

#Calibrate Arm
arm.calibrateArm()

while((not arm.armCalibrated()) or(arm.checkMovement())):
    time.sleep(1)

while arm.connected:

    #arm.positionJoints(fixedPosition)
    arm.standUp()

    while(arm.checkMovement()):
        time.sleep(0.5)

    #Select Random position for each joint
    randomPosition = [0]*arm.joints
    for i in range(0,arm.joints):
        randomPosition[i] = arm.selectRandomPosition(i)

    #Move arm to a random position
    arm.positionJoints(randomPosition)
    
    while(arm.checkMovement()):
        time.sleep(0.5)
    
arm.stop()