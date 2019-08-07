#NAME: dance.py
#DATE: 14/06/2019
#AUTH: Ryan McCartney
#DESC: Python function to control a 6DOF robotic arm with simple scripting
#COPY: Copyright 2019, All Rights Reserved, Ryan McCartney

import sys
sys.path.append('control/')
from arm import Arm
import time
import json

#Clear Log File
open('logs/log.txt', 'w').close()

with open('config/config.json') as json_file:  
    config = json.load(json_file)

arm = Arm(config)
angle = 70

arm.calibrateArm()
while not arm.armCalibrated():
    time.sleep(0.5)

while arm.checkMovement():
    time.sleep(0.5)

arm.setSpeed(0,90)
arm.setSpeed(1,90)
arm.setSpeed(2,90)
arm.setSpeed(3,200)
arm.setSpeed(4,200)
arm.setSpeed(5,200)

while arm.connected:

    arm.moveJoint(0,angle)
    arm.moveJoint(1,angle)
    arm.moveJoint(2,angle)
    arm.moveJoint(3,angle)
    arm.moveJoint(4,angle)
    arm.moveJoint(5,angle)
    
    time.sleep(5)

    arm.moveJoint(0,-angle)
    arm.moveJoint(1,-angle)
    arm.moveJoint(2,-angle)
    arm.moveJoint(3,-angle)
    arm.moveJoint(4,-angle)
    arm.moveJoint(5,-angle)

    time.sleep(5)
    
arm.stop()