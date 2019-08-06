#NAME: testForwardKinematic.py
#DATE: 05/06/2019
#AUTH: QLab Makerspace
#DESC: Script to demonstrate how to properly to interact with the Kinematic Class
#COPY: Copyright 2019, All Rights Reserved

import sys
sys.path.append('control/')
from kinematics import Kinematic
import time

#instantiate kinematic object
kin = Kinematic()

#input joint positions (work out why there is angles added and subtracted from 2 joints? caution when testing)
joint_positions = [0.01,-90,(90-90),0.01,0.01,(0.01+180)]
#[J0,J1,J2,J3,J4,J5]

print("INFO: Running Forward Kinematics")
startTime = time.time()

#perform forward kinematics and extract end effector pose
pose = kin.forwardKinematics(joint_positions)
#pose =  [X,Y,Z,Yaw,Pitch,Roll]

print("INFO: Pose is '"+str(pose)+"'.")
print("INFO: Forward Kinematic Test complete. Processing took "+str(round(time.time()-startTime,2))+" seconds.")