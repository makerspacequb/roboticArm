#script to demonstrate how to properly to interact with the kinematicSolver

from kinematicSolver import Kinematic

#instantiate kinematic object
kin = Kinematic()

#input joint positions (work out why there is angles added and subtracted from 2 joints? caution when testing)
# [J0,J1,J2,J3,J4,J5]
joint_positions = [0.01,-90,(90-90),0.01,0.01,(0.01+180)]

print("Running Forward Kinematics")
#perform forward kinematics and extract end effector pose
pose = kin.forwardKinematics(joint_positions)
#pose =  [X,Y,Z,Yaw,Pitch,Roll]
print(pose)
print("Task complete")


#inverse kinematics not finished
