
import sys
sys.path.append('control/')
from arm import Arm
from kinematics import Kinematic
import time
import json

global config

kin = Kinematic()

#Clear Log File
open('logs/log.txt', 'w').close()

with open('config/config.json') as json_file:  
    config = json.load(json_file)

#Create instance of Arm class
#ip_address = "192.168.0.105"
arm = Arm(config)
arm.reset()

arm.calibrateArm()
step = 100

while((not arm.armCalibrated()) or (arm.checkMovement())):
    time.sleep(1)
i = 0

#kin_angles = [0, 0, 0, 10, 10, 10]
kin_angles = [-0.02 ,-90.01, 90.01, 0.02, 89.9, -0.02] #joints into spreadsheet

result = kin.check_limits(kin_angles) #check the angles are within the model ranges

if result == 1:
    cj = kin.kin2native_angles(kin_angles) #convert to native machine joint angles

    t = 0.0
    while arm.connected:# and i < 4:
        
        print("top of process")
        #arm.positionJoints(cj)


        arm.moveJointTo(0,cj[0])
        time.sleep(t)
        arm.moveJointTo(1,cj[1])
        time.sleep(t)
        arm.moveJointTo(2,cj[2])
        time.sleep(t)
        arm.moveJointTo(3,cj[3])
        time.sleep(t)
        arm.moveJointTo(4,cj[4])
        time.sleep(t)
        arm.moveJointTo(5,cj[5])
        
        print("cj", cj)
        
        pose = kin.forwardKinematics(cj)
        
        last_valid_pose = list(pose)
        print("pose: ", pose)
    
        try:
            d = input("Enter 12 to move in x direction, 34 to move in y direction, 56 for z direction\n")
        except:
            d=7   

        if d == 1:
            pose[1] = pose[1] - step #move along y axis
        elif d == 2:
            pose[1] = pose[1] + step #move along y axis
        elif d == 3:
            pose[0] = pose[0] - step #move along x axis
        elif d == 4:
            pose[0] = pose[0] + step #move along x axis
        elif d == 5:
            pose[2] = pose[2] - step #move along z axis
        elif d == 6:
            pose[2] = pose[2] + step #move along z axis
        else:
            print("invalid input")

        try:
            input_cj = list(cj)
            cj = kin.inverseKinematics(pose, cj)
            
            if cj == -1: # if no solution is found dont adjust cj or pose
                cj = list(input_cj)
                pose = list(last_valid_pose)

            print("new angles ", kin.native2kin_angles(cj))
        except:
            print("could not print for some reason", cj)


    arm.standUp()

    while((not arm.armCalibrated()) or (arm.checkMovement())):
        time.sleep(1)

    arm.stop()
else:
    print("Initial arrangment is invalid")