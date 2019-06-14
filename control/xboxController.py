#NAME: xboxController.py
#DATE: 14/06/2019
#AUTH: Ryan McCartney
#DESC: Python function to control a 6DOF robotic arm with an xbox controller.
#COPY: Copyright 2019, All Rights Reserved, Ryan McCartney

from arm import Arm
import pygame
import time

ip_address = "roboticArm.local"
arm = Arm(ip_address)
gamepadConnected = False

while 1:

    while arm.connected:

        while gamepadConnected:

            #Contol the Robotic Arm with the xbox controller.
    


        try:
            #Check for an xbox controller and connect if avalible.
            gamepadConnected = True
        except:
            gamepadConnected = False

    if arm.connected == False:
        #Poll connecion
        arm.checkConnection()
        print("ERROR: Connection lost with robotic arm. Waiting for reconnect...")
        time.delay(1)
