#NAME: standSit.py
#DATE: 14/06/2019
#AUTH: Ryan McCartney
#DESC: Python function to control a 6DOF robotic arm with simple scripting
#COPY: Copyright 2019, All Rights Reserved, Ryan McCartney

from arm import Arm
import time
import json

global config

#Clear Log File
open('control/log.txt', 'w').close()

with open('control/config.json') as json_file:  
    config = json.load(json_file)

#Create instance of Arm class
ip_address = "192.168.0.105"
arm = Arm(ip_address,config)
arm.reset()

arm.calibrateArm()
time.sleep(30)

while(arm.armCalibrated() == False):
    time.sleep(1)

while arm.connected:
        
    while(arm.checkMovement()):
        time.sleep(1)

    arm.standUp()
    time.sleep(10)
    
    while(arm.checkMovement()):
        time.sleep(1)

    arm.lieDown()
    time.sleep(10)
    
arm.stop()