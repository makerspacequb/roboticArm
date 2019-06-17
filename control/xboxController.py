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

#Start Pygame
pygame.init()
pygame.joystick.init()
gamepad = 0

def mapCommands(leftAnalogue, rightAnalogue, aButton, bButton):

    command = "q"
    
    return command

while 1:

    while arm.connected:

        while gamepads > 0:

            #Contol the Robotic Arm with the xbox controller.
            try:
                #Initialise first gamepad
                j = pygame.joystick.Joystick(0)
                j.init()
            
                #Check axis avalible
                axis = j.get_numaxes()
            
                print("INFO: Controller connected.")

                while 1:
                              #Get Current Data
                    pygame.event.get()

                    xAxisLeft = j.get_axis(0)
                    yAxisLeft = j.get_axis(1)
                    aButton = j.get_button(0)
                    bButton = j.get_button(1)
                    yButton = j.get_button(2)
                    xButton = j.get_button(3)

                    command = mapCommands(xAxisLeft,yAxisLeft)
                    arm.sendCommand(command)
            except:
                gamepads = 0
                print("ERROR: Controller connection lost.")
                pass

        #Check for an xbox controller
        gamepads = pygame.joystick.get_count()
        time.delay(1)
        
    if arm.connected == False:
        #Poll connecion
        arm.checkConnection()
        print("ERROR: Connection lost with robotic arm. Waiting for reconnect...")
        time.delay(1)
