#NAME: random.py
#DATE: 14/06/2019
#AUTH: Ryan McCartney
#DESC: Python function to control a 6DOF robotic randomly moving and then returning to a point
#COPY: Copyright 2019, All Rights Reserved, Ryan McCartney

from arm import Arm
import random
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
    
    #Set Position
    arm.moveTo(0,100)
    time.sleep(1)

    arm.moveTo(2,-90)
    time.sleep(2)
    
    arm.moveTo(1,45)
    arm.moveTo(3,90)
    arm.moveTo(4,90)
    arm.moveTo(5,45)

    #Wait 3 Seconds
    time.sleep(3)

    #Make a random move
    degrees = random.randint(0,arm.jointPositionMax[0])
    arm.move(0,angle)
    degrees = random.randint(0,arm.jointPositionMax[1])
    arm.move(1,angle)
    degrees = random.randint(0,arm.jointPositionMax[2])
    arm.move(2,angle)
    degrees = random.randint(0,arm.jointPositionMax[3])
    arm.move(3,angle)
    degrees = random.randint(0,arm.jointPositionMax[4])
    arm.move(4,angle)
    degrees = random.randint(0,arm.jointPositionMax[5])
    arm.move(5,angle)

    time.sleep(5)
arm.stop()