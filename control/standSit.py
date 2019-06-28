#NAME: standSit.py
#DATE: 14/06/2019
#AUTH: Ryan McCartney
#DESC: Python function to control a 6DOF robotic arm with simple scripting
#COPY: Copyright 2019, All Rights Reserved, Ryan McCartney

from arm import Arm
import time

#Clear Log File
open('control/log.txt', 'w').close()

#Create instance of Arm class
ip_address = "192.168.0.105"
arm = Arm(ip_address)
arm.reset()

arm.speed(0,50)
arm.speed(1,50)
arm.speed(2,50)
arm.speed(3,50)
arm.speed(4,50)
arm.speed(5,50)

arm.calibrateArm()

while(arm.armCalibrated() == False):
    time.sleep(1)

while arm.connected:
    
    arm.standUp()

    while(arm.checkMovement()):
        time.sleep(1)

    arm.lieDown()

    while(arm.checkMovement()):
        time.sleep(1)
    
arm.stop()