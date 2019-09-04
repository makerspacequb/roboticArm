
import sys
sys.path.append('control/')
from arm import Arm
from kinematics import Kinematic
import time
import json
from xboxControl import XboxController


global config

ControlId = -1
ControlVal = 0#set initial state
#=================================================
#define callback function to extract control actions
def controlCallBack(xboxControlId, value):
    global ControlId, ControlVal
    ControlId = xboxControlId
    ControlVal = value
    
#setup xbox controller, set out the deadzone and scale, also invert the Y Axis (for some reason in Pygame negative is up - wierd! 
xboxCont = XboxController(controlCallBack, deadzone = 30, scale = 100, invertYAxis = True)

#start the xbox controller
xboxCont.start()
#=================================================

kin = Kinematic()

#Clear Log File
open('logs/log.txt', 'w').close()

with open('config/config.json') as json_file:  
    config = json.load(json_file)

#Create instance of Arm class for each arm
arm1 = Arm(config)
arm2 = Arm(config, "192.168.0.106")

arm = []
arm.append(arm1)
arm.append(arm2)

arm[0].resetEStop()
arm[1].resetEStop()

#calibrate both arms
arm[0].calibrateArm()
arm[1].calibrateArm()

while(not (arm[0].armCalibrated() and arm[1].armCalibrated())):
    time.sleep(1)
    print("waiting to calibrate...")

i = 0

#kin_angles = [0, 0, 0, 10, 10, 10]
kin_angles = [-0.02 ,-90.01, 90.01, 0.02, 89.9, -0.02] #joints into spreadsheet
step = 100
angle_step = 5
result = kin.check_limits(kin_angles) #check the angles are within the model ranges

if result == 1:
    
    cj0 = kin.kin2native_angles(kin_angles) #convert to native machine joint angles
    cj1 = kin.kin2native_angles(kin_angles) #convert to native machine joint angles

    first_loop = True
    step_needed = True
    arm_index = 0 #initially select left arm

    while arm[0].connected and arm[1].connected:

        if arm_index == 0:
            cj = list(cj0)
        elif arm_index == 1:
            cj = list(cj1)

        if step_needed == True:
            print("moving...")

            if first_loop == True:
                arm[0].positionJoints(cj)
                arm[1].positionJoints(cj)
            else:                
                arm[arm_index].positionJoints(cj)


            first_loop= False
            step_needed = False

        pose = kin.forwardKinematics(cj)

        last_valid_pose = list(pose)
        input_cj = list(cj)

        #move in the xy plane
        if ControlId == 0 and (ControlVal > 50 or ControlVal < -50):
            if  ControlVal < -50:
                pose[1] = pose[1] - step #move along y axis
                step_needed = True
            if ControlVal > 50:
                pose[1] = pose[1] + step #move along y axis
                step_needed = True
        if ControlId == 1 and (ControlVal > 50 or ControlVal < -50):
            if ControlVal < -50:
                pose[0] = pose[0] - step #move along x axis
                step_needed = True
            if ControlVal > 50:
                pose[0] = pose[0] + step #move along x axis
                step_needed = True

        #move in the xy plane
        if ControlId == 17 and ControlVal !=0:
            if  ControlVal[0] == -1:
                pose[3] = pose[3] - angle_step #move along y axis
                step_needed = True
            if ControlVal[0] == 1:
                pose[3] = pose[3] + angle_step #move along y axis
                step_needed = True
            if ControlVal[1] == -1:
                pose[4] = pose[4] - angle_step #move along x axis
                step_needed = True
            if ControlVal[1] == 1:
                pose[4] = pose[4] + angle_step #move along x axis
                step_needed = True
        
        #move down (z axis)
        if ControlId == 6 and ControlVal == 1:
            pose[2] = pose[2] - step #move along z axis
            step_needed = True
        
        #move up (z axis)
        if ControlId == 9 and ControlVal == 1:
            pose[2] = pose[2] + step #move along z axis
            step_needed = True

        #adjust step size (make smaller) (for finer control)
        if ControlId == 10 and ControlVal == 1:
            if step > 0:
                step = step - 1
                print("step size is now " + str(step) + "mm")
            else:
                print("step cannot be made any smaller than 0mm")
        
        #adjust step size (make larger) (for rougher control)
        if ControlId == 11 and ControlVal == 1:
            if step < 400:
                step = step + 1
                print("step size is now " + str(step) + "mm")
            else:
                print("step cannot be made any larger than 400mm")

        if ControlId == 8 and ControlVal == 1:
            print("arm1 selected...")
            arm_index = 0
            step_needed = False

        if ControlId == 7 and ControlVal == 1:
            print("arm2 selected...")
            arm_index = 1
            step_needed = False


        if ControlId == 12:
            break

        if step_needed == True:            
            try:

                cj = kin.inverseKinematics(pose, cj)
                
                if cj == -1: # if no solution is found dont adjust cj or pose
                    print("no valid solution found")
                    cj = list(input_cj)
                    pose = list(last_valid_pose)

                print("new angles ", kin.native2kin_angles(cj))
                
                #save the new angles to the correct arm
                if arm_index == 0:
                    cj0 = list(cj)
                elif arm_index == 1:
                    cj1 = list(cj)

            except:
                print("could not print for some reason", cj)
        
    arm[0].standUp()
    arm[1].standUp()
    
else:
    print("Initial arrangment is invalid")

