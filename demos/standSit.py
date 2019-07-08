#NAME: standSit.py
#DATE: 14/06/2019
#AUTH: Ryan McCartney
#DESC: Python script moving the AR2 between upright and resting
#COPY: Copyright 2019, All Rights Reserved, Ryan McCartney

import sys
sys.path.append('control/')
from arm import Arm
import time
import json

global config

#Clear Log File
open('logs/log.txt', 'w').close()

with open('config/config.json') as json_file:  
    config = json.load(json_file)

#Create instance of Arm class
ip_address = "192.168.0.105"
arm = Arm(ip_address,config)
arm.reset()

arm.calibrateArm()

while((not arm.armCalibrated()) or(arm.checkMovement())):
    time.sleep(1)

while arm.connected:
        
    arm.waitToStationary()
    arm.standUp()

    arm.waitToStationary()
    arm.rest()
    
arm.stop()