#NAME: dance.py
#DATE: 14/06/2019
#AUTH: Ryan McCartney
#DESC: Python function to control a 6DOF robotic arm with simple scripting
#COPY: Copyright 2019, All Rights Reserved, Ryan McCartney

from arm import Arm
import time

ip_address = "192.168.0.101"
arm = Arm(ip_address)

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
    
    time.sleep(3)
    arm.move(0,-angle)
    arm.move(1,-angle)
    arm.move(2,-angle)
    arm.move(3,-angle)
    arm.move(4,-angle)
    arm.move(5,-angle)
    time.sleep(3)
    
arm.stop()