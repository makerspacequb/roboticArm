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

arm.calibrateArm()
while(arm.armCalibrated() == False):
    time.sleep(1)

angle = 45

arm.speed(0,50)
arm.speed(1,50)
arm.speed(2,50)
arm.speed(3,50)
arm.speed(4,50)
arm.speed(5,50)

while arm.connected:
    arm.move(0,angle)
    arm.move(1,angle)
    arm.move(2,angle)
    arm.move(3,angle)
    arm.move(4,angle)
    arm.move(5,angle)
    
    time.sleep(5)

    arm.move(0,-angle)
    arm.move(1,-angle)
    arm.move(2,-angle)
    arm.move(3,-angle)
    arm.move(4,-angle)
    arm.move(5,-angle)

    time.sleep(5)
    
arm.stop()