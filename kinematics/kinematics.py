import numpy as np
import time
import math
import threading

#define threading wrapper
def threaded(fn):
    def wrapper(*args, **kwargs):
        thread = threading.Thread(target=fn, args=args, kwargs=kwargs)
        thread.start()
        return thread
    return wrapper
    
class Kinematic:
    
    def __init__(self):

        self.select_workFrame()#select a base reference coordinate frame
        self.select_tool()#select to tool frame
        #self.calculate_joint_matrices()#generate joint matrices 


    #Generate Work Frame Matrix from the X,Y,Z,Roll,Pitch,Yaw Reference
    def generateWorkFrame(self):
        #workframe is the reference origin for the robot 
        #workframe - X,Y,Z,yaw(x),pitch(y),roll(z)

        wf_data = np.array([0,0,0,0,0,0], dtype=float)
        
        workFrame = np.zeros((4,4), dtype=float)

        #build the 4x4 workFrame matrix
        #first row
        workFrame [0,0] = math.cos(wf_data[5])*math.cos(wf_data[4])
        workFrame [0,1] = math.sin(wf_data[5])*math.cos(wf_data[3])+math.cos(wf_data[5])*math.sin(wf_data[4])*math.sin(wf_data[3])
        workFrame [0,2] = math.sin(wf_data[5])*math.sin(wf_data[3]) + math.cos(wf_data[5])*math.sin(wf_data[4])*math.cos(wf_data[3])
        workFrame [0,3] = wf_data[0]
        #second row
        workFrame [1,0] = math.sin(wf_data[5])*math.cos(wf_data[4])
        workFrame [1,1] = math.cos(wf_data[5])*math.cos(wf_data[3])+math.sin(wf_data[5])*math.sin(wf_data[4])*math.sin(wf_data[3])
        workFrame [1,2] = math.cos(wf_data[5])*math.sin(wf_data[3]) + math.sin(wf_data[5])*math.sin(wf_data[4])*math.cos(wf_data[3])
        workFrame [1,3] = wf_data[1]
        #third row
        workFrame [2,0] = math.sin(wf_data[5])
        workFrame [2,1] = math.cos(wf_data[4])*math.sin(wf_data[3])
        workFrame [2,2] = math.cos(wf_data[4])*math.cos(wf_data[3])
        workFrame [2,3] = wf_data[2]
        #fourth row
        workFrame [3,0] = 0
        workFrame [3,1] = 0
        workFrame [3,2] = 0
        workFrame [3,3] = 1
        
        return workFrame

    def generateToolFrame(self,tool):
        #this function sets the end effector frame based on the X,Y,Z,Yaw,Pitch,Roll offset from last joint 

        toolFrame = np.zeros((4,4), dtype=float)

        #build the 4x4 toolframe matrix
        #first row
        toolFrame [0,0] = math.cos(tool[5])*math.cos(tool[4])
        toolFrame [0,1] = math.sin(tool[5])*math.cos(tool[3])+math.cos(tool[5])*math.sin(tool[4])*math.sin(tool[3])
        toolFrame [0,2] = math.sin(tool[5])*math.sin(tool[3]) + math.cos(tool[5])*math.sin(tool[4])*math.cos(tool[3])
        toolFrame [0,3] = tool[0]
        #second row
        toolFrame [1,0] = math.sin(tool[5])*math.cos(tool[4])
        toolFrame [1,1] = math.cos(tool[5])*math.cos(tool[3])+math.sin(tool[5])*math.sin(tool[4])*math.sin(tool[3])
        toolFrame [1,2] = math.cos(tool[5])*math.sin(tool[3]) + math.sin(tool[5])*math.sin(tool[4])*math.cos(tool[3])
        toolFrame [1,3] = tool[1]
        #third row
        toolFrame [2,0] = math.sin(tool[5])
        toolFrame [2,1] = math.cos(tool[4])*math.sin(tool[3])
        toolFrame [2,2] = math.cos(tool[4])*math.cos(tool[3])
        toolFrame [2,3] = tool[2]
        #fourth row
        toolFrame [3,0] = 0
        toolFrame [3,1] = 0
        toolFrame [3,2] = 0
        toolFrame [3,3] = 1

        return toolFrame

    def extract_pose(self, H_matrix):
        #Convert a H_matrix to X,Y,Z,Yaw,Pitch,Roll

        position = H_matrix[0:3,3] 

        Pitch =np.arctan2( math.sqrt(H_matrix[0,2]**2 + H_matrix[1,2]**2), -1*H_matrix[2,2]) 
        Yaw = np.arctan2(H_matrix[2,0]/Pitch, H_matrix[2,1]/Pitch)
        Roll = np.arctan2(H_matrix[0,2]/Pitch,H_matrix[1,2]/Pitch)
 
        pose = [position[0], position[1], position[2], Yaw, Pitch, Roll]
        return pose

    def select_tool(self):
        #method can be expanded to allow different end effectors
        #set the endpoint of the robot equal to the end effector position
        self.toolFrame = self.generateToolFrame([0,0,0,0,0,0])

    def select_workFrame(self):
        #select the base frame to refer all points
        #can potentially be expanded for a mobile base arrangement
        self.workFrame = self.generateWorkFrame()

    def calculate_A_matrix(self, joint_angles):
        #generate matrices containing information on how each joint is related based on joint angles
        
        #Denavit Hartenberg parameters
        d = [169.77,0,0,-222.63,0,-36.25] #Link offset Matrix (Arm Lengths)
        a = [64.2,305,0,0,0,0] #Link length
        alpha = np.radians([-90,0,90,-90,90,0])#Link twist
        theta = np.radians(joint_angles)#Joint angles

        A_matrix= np.zeros((6,4,4), dtype=float)
        #calculate all joint matrices
        i = 0
        while(i < A_matrix.shape[0]):
            #first row
            A_matrix[i,0,0] = math.cos(theta[i])
            A_matrix[i,0,1] = -1*math.sin(theta[i])*math.cos(alpha[i])
            A_matrix[i,0,2] = math.sin(theta[i])*math.sin(alpha[i])
            A_matrix[i,0,3] = a[i]*math.cos(theta[i])
            #second row
            A_matrix[i,1,0] = math.sin(theta[i])
            A_matrix[i,1,1] = math.cos(theta[i])*math.cos(alpha[i])
            A_matrix[i,1,2] = -1*math.cos(theta[i])*math.sin(alpha[i])
            A_matrix[i,1,3] = a[i]*math.sin(theta[i])
            #third row
            A_matrix[i,2,0] = 0
            A_matrix[i,2,1] = math.sin(alpha[i])
            A_matrix[i,2,2] = math.cos(alpha[i])
            A_matrix[i,2,3] = d[i]
            #fourth row
            A_matrix[i,3,0] = 0
            A_matrix[i,3,1] = 0
            A_matrix[i,3,2] = 0
            A_matrix[i,3,3] = 1

            i = i + 1

            self.A_matrix = A_matrix

    def calculate_H_matrix(self):
        #generate a list of matrices relating each joint back to the work frame based on joint angles
        #the last H_matrix in the list will be the relating to the end effector pose

        #create array of all the joints relationship to the work frame
        H_matrix = np.zeros((7,4,4), dtype=float)

        #perform the coordinate transformation from the base frame to J0
        H_matrix[0] = self.workFrame.dot(self.A_matrix[0])

        i = 1
        #apply joint transformations to move from base to end of arm
        #each element of H_matrix contains the information relating to a joints pose in space
        while ( i < H_matrix.shape[0]-1):
            H_matrix[i] = H_matrix[i-1].dot(self.A_matrix[i])
            i = i + 1

        H_matrix[i] = H_matrix[i-1].dot(self.toolFrame)

        
        self.H_matrix = H_matrix


    def forwardKinematics(self, joint_angles):
        #this method applies the Denavit Hartenberg convention to perform formward kinematics
        
        self.calculate_A_matrix(joint_angles)
        self.calculate_H_matrix()

        #pass the final value of the chain and extract th pose of the end effector
        pose = self.extract_pose(self.H_matrix[self.H_matrix.shape[0]-1])

        #pose is of the form [X,Y,Z,Yaw, Pitch, Roll]
        return pose

    def inverseKinematics(self,pose):
        #method to convert a desired pose to joint positions
        
        return 0
