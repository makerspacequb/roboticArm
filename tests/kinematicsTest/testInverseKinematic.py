#NAME: testForwardKinematic.py
#DATE: 05/06/2019
#AUTH: QLab Makerspace
#DESC: Script to demonstrate how to properly to interact with the Kinematic Class
#COPY: Copyright 2019, All Rights Reserved

import sys
sys.path.append('control/')
from kinematics import Kinematic

#instantiate kinematic object
kin = Kinematic()

"""
#input joint positions (work out why there is angles added and subtracted from 2 joints? caution when testing)
# [J0,J1,J2,J3,J4,J5]
joint_positions = [0.01,-90,(90-90),0.01,0.01,(0.01+180)]

print("Running Forward Kinematics")
#perform forward kinematics and extract end effector pose
pose = kin.forwardKinematics(joint_positions)
#pose =  [X,Y,Z,Yaw,Pitch,Roll]
print(pose)
print("Task complete")
"""

#insert a desired pose for inverse kinematics
current_joints = [30, 90, 90, 30, -30, 30]
desired_pose = [14.65766456, 43.51281973, 317.0026196, -163.0643134, 14.87094445, 106.9356866]

#inverse kinematics not finished
Joints = kin.inverseKinematics(desired_pose, current_joints)

print(Joints)