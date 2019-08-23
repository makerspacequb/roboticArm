import numpy as np
import time
import math

dt =  np.dtype('Float64') #define array data type

class Kinematic:
    
    def __init__(self):

        self.select_workFrame()#select a base reference coordinate frame
        self.select_tool()#select to tool frame

        #initialise Denavit Hartenberg parameters
        self.d = [169.77,0,0,-222.63,0,-36.25] #Link offset Matrix (Arm Lengths)
        self.a = [64.2,305,0,0,0,0] #Link length
        self.alpha = np.radians([-90,0,90,-90,90,0])#Link twist
        
        #self.calculate_joint_matrices()#generate joint matrices 

    def select_workFrame(self):
        #select the base frame to refer all points
        #can potentially be expanded for a mobile base arrangement
        self.workFrame = self.generateWorkFrame()

    def select_tool(self):
        #method can be expanded to allow different end effectors
        #set the endpoint of the robot equal to the end effector position
        self.toolFrame = self.generateToolFrame([0,0,0,0,0,0])

    #Generate Work Frame Matrix from the X,Y,Z,Roll,Pitch,Yaw Reference
    def generateWorkFrame(self):
        #workframe is the reference origin for the robot 
        #workframe - X,Y,Z,yaw(x),pitch(y),roll(z)

        """
        workframe = [a, b, x, d, e, f]

                    |cos(f)cos(e)    ,  sin(f)cos(d) + cos(f)sin(e)sin(d)   , sin(f)cos(d) + cos(f)sin(e)cos(d)     , a     |
                    |sin(f)cos(d)    ,  sin(f)cos(e) + sin(f)sin(e)sin(d)   , cos(f)sin(d) + sin(f)sin(e)cos(e)     , b     |
        #A_matrix = |sin(f)          ,  cos(e)sin(d)                        ,  cos(e)cos(d)                         , c     |
                    |0               ,        0                             ,0                                      , 1     |

        """
        wf_data = np.array([0,0,0,0,0,0], dtype=dt)
        workFrame = np.zeros((4,4), dtype=dt)

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
        """
        tool = [a, b, c, d, e, f]

                    |cos(f)cos(e)    ,  sin(f)cos(d) + cos(f)sin(e)sin(d)   , sin(f)cos(d) + cos(f)sin(e)cos(d)     , a     |
                    |sin(f)cos(d)    ,  sin(f)cos(e) + sin(f)sin(e)sin(d)   , cos(f)sin(d) + sin(f)sin(e)cos(e)     , b     |
        #A_matrix = |sin(f)          ,  cos(e)sin(d)                        ,  cos(e)cos(d)                         , c     |
                    |0               ,        0                             ,0                                      , 1     |
        """
        toolFrame = np.zeros((4,4), dtype=dt)

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
 
        pose = [position[0], position[1], position[2], math.degrees(Yaw), math.degrees(Pitch), math.degrees(Roll)]
        return pose

    def calculate_A_matrix(self, joint_angles):
        #generate matrices containing information on how each joint is related based on joint angles and link lengths

        #the form of the matrices generated are:
        #let joint_angles[element] = j

        """
                    |cos(j), -sin(j)cos(alpha)  , sin(j)sin(alpha)  , acos(j)   |
                    |sin(j), cos(j)cos(alpha)   , -cos(j)sin(alpha) , asin(j)   |
        #A_matrix = |0     , sin(alpha)         ,  cos(alpha        ,d          |
                    |0     , 0                  ,0                  ,1          |

        """

        #joint angles are input in degrees so convert them to radians
        theta = np.radians(joint_angles)#Joint angles

        #save constant DH parameters
        d = self.d
        a = self.a
        alpha = self.alpha

        A_matrix= np.zeros((len(joint_angles),4,4), dtype=dt)
        #calculate all joint matrices
        i = 0
        while(i < len(joint_angles)):
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
            #print(i)
            #print(A_matrix[i])
            i = i + 1
            
        return A_matrix

    def calculate_H_matrix(self, A_matrix):
        #generate a list of matrices relating each joint back to the work frame based on joint angles
        #the last H_matrix in the list will be the relating to the end effector pose
        #create array of all the joints relationship to the work frame
        
        H_matrix = np.zeros((7,4,4), dtype=dt)

        #perform the coordinate transformation from the base frame to J0
        H_matrix[0] = self.workFrame.dot(A_matrix[0])
        i = 1
        
        #apply joint transformations to move from base to end of arm
        #each element of H_matrix contains the information relating to a joints pose in space
        
        while ( i < H_matrix.shape[0]-1):
            H_matrix[i] = H_matrix[i-1].dot(A_matrix[i])
            i = i + 1

        H_matrix[i] = H_matrix[i-1].dot(self.toolFrame)

        return H_matrix

    def forwardKinematics(self, joint_angles):
        #this method applies the Denavit Hartenberg convention to perform formward kinematics
        
        #add the arbitrary offsets (may need removed)
        #joint_angles[2] = joint_angles[2] - 90 
        #joint_angles[5] = joint_angles[5] + 180
        
        #make a copy of joint angles so the passed in value is unaffected

        j = self.native2kin_angles(joint_angles) #list(joint_angles)
        
        result =  self.check_limits(j)

        if result == 1: #valid input given
                
            #swap range of inputs
            j[0] =  j[0]# - 180 
            j[1] =  j[1]# - 180
            j[2] = (j[2]- 90)# - 180
            j[3] =  j[3]# - 180
            j[4] =  j[4]# - 180
            j[5] = (j[5] + 180)# - 180

            A_matrix = self.calculate_A_matrix(j)
            H_matrix = self.calculate_H_matrix(A_matrix)

            #pass the final value of the chain and extract th pose of the end effector
            pose = self.extract_pose(H_matrix[H_matrix.shape[0]-1])

            #pose is of the form [X,Y,Z,Yaw, Pitch, Roll]
            return pose
        else:
            return -1 #invalid input

    def calculate_R0T_Matrix(self, pose):
        #calculate ROT matrix
        """
        pose = [a, b, c, d, e, f]

                    |cos(d)cos(f)-cos(e)sin(d)sin(f)    ,   cos(f)sin(d)+cos(d)cos(e)sin(f) , sin(e)cos(f)  , a |
                    |cos(e)cos(f)sin(d)+cos(d)sin(f)    ,   cos(d)cos(e)sin(f)-sin(d)sin(f) , -cos(d)sin(e) , b |
        #A_matrix = |sin(d)sin(e)                       ,   cos(d)sin(e)                    , -cos(e)       , c |
                    |0                                   ,  0                               , 0             , 1 |
        """
        #create 4x4 matrix
        R0T_matrix= np.zeros((4,4), dtype=dt)
        #print("b ", pose[1])
        #first row
        R0T_matrix[0,0] = math.cos(pose[3])*math.cos(pose[5]) - math.cos(pose[4])*math.sin(pose[3])*math.sin(pose[5])
        R0T_matrix[0,1] = math.cos(pose[5])*math.sin(pose[3]) + math.cos(pose[3])*math.cos(pose[4])*math.sin(pose[5])
        R0T_matrix[0,2] = math.sin(pose[4])*math.sin(pose[5])
        R0T_matrix[0,3] = pose[0]
        #second row
        R0T_matrix[1,0] = math.cos(pose[4])*math.cos(pose[5])*math.sin(pose[3]) + math.cos(pose[3])*math.sin(pose[5])
        R0T_matrix[1,1] = math.cos(pose[3])*math.cos(pose[4])*math.cos(pose[5]) - math.sin(pose[3])*math.sin(pose[5]) 
        R0T_matrix[1,2] = math.cos(pose[5])*math.sin(pose[4])
        R0T_matrix[1,3] = pose[1]
        #third row
        R0T_matrix[2,0] = math.sin(pose[3])*math.sin(pose[4])
        R0T_matrix[2,1] = math.cos(pose[3])*math.sin(pose[4]) 
        R0T_matrix[2,2] = -1*math.cos(pose[4])
        R0T_matrix[2,3] = pose[2]
        #fourth row
        R0T_matrix[3,0] = 0
        R0T_matrix[3,1] = 0 
        R0T_matrix[3,2] = 0
        R0T_matrix[3,3] = 1


        return R0T_matrix

    def gen_R06_remove(self):
        #define transformation matrix to move from the point on the arm where the tool attaches to the wrist centre
        
        """
                    |cos(j) , -sin(j)cos(alpha)  , sin(j)sin(alpha)  , acos(j)    |
                    |sin(j) , cos(j)cos(alpha)   , -cos(j)sin(alpha) , asin(j)    |
        #A_matrix = |0      , sin(alpha)         , cos(alpha)        , d          |
                    |0      , 0                  , 0                 , 1          |

        """        
        
        #create 4x4 matrix
        R06_remove= np.zeros((4,4), dtype=dt)
        
        theta = math.pi
        alpha = 0
        d = -36.35
        a = 0

        #Transposed version of the rotated part of the DH matrix
        #negtive positional parts of of the DH matrix
         
        #first row
        R06_remove[0,0] = math.cos(theta)
        R06_remove[0,1] = math.sin(theta)
        R06_remove[0,2] = 0
        R06_remove[0,3] = -1*a*math.cos(theta)
        #second row

        R06_remove[1,0] = -1*math.sin(theta)*math.cos(alpha)
        R06_remove[1,1] = math.cos(theta)*math.cos(alpha)
        R06_remove[1,2] = math.sin(alpha)
        R06_remove[1,3] = -1*a*math.sin(theta)
        #third row
        
        R06_remove[2,0] = math.sin(theta)*math.sin(alpha)
        R06_remove[2,1] = -1*math.cos(theta)*math.sin(alpha) 
        R06_remove[2,2] = math.cos(alpha)
        R06_remove[2,3] = -1*d
        #fourth row
        R06_remove[3,0] = 0
        R06_remove[3,1] = 0 
        R06_remove[3,2] = 0
        R06_remove[3,3] = 1

        return R06_remove

    def calc_J1(self, pose, R05):
        #calculate J1 position

        #extract desired xy coordinates
        x = pose[0]
        y = pose[1]
        #print("print ========", x, y)
        J1 = math.atan(R05[1,3]/R05[0,3])
        #print(R05[1,3])
        #print(R05[0,3])

        #calculate quadrant
        if(x > 0 and y > 0):
            print("Quadrant 1")
            J1 = math.degrees(J1)
        elif(x > 0 and y < 0):
            print("Quadrant 2")
            J1 = math.degrees(J1)
        elif(x < 0 and y < 0):
            print("Quadrant 3")
            J1 = -180 + math.degrees(J1)
        elif(x < 0 and y > 0):
            print("Quadrant 4")
            J1 = 180 + math.degrees(J1) 
        else:
            J1 = 0

        return J1

    def calc_J2J3(self, pose, R05):
        #calculate joint positions J2 and J3
        
        #form table of useful parameters of the form (based on labelled diagram):
        #this table performs all the nessecary trig for calculating J2 and J3
        
        """
                   fwd     mid
        px     |
        py     |
        px-a1  |
        pa2H   |
        pa3H   |
        thetaA |
        thetaB |
        thetaC |
        thetaD |
        thetaE |
        J1angle|
        J2angle|
        """
        try:
            table = np.zeros((12,2), dtype=dt)
            
            table[0,0] = math.sqrt(R05[1,3]**2 +R05[0,3]**2) 
            table[0,1] = table[0,0]  

            table[1,0] = R05[2,3] - self.d[0]
            table[1,1] = table[1,0]

            table[2,0] = table[0,0] - self.a[0]
            table[2,1] = self.a[0] - table[0,0]

            table[3,0] = math.sqrt(table[1,0]**2 + table[2,0]**2)  
            table[3,1] = math.sqrt(table[1,1]**2 + table[2,1]**2)

            table[4,0] = math.sqrt(self.d[3]**2 + self.a[3]**2)  
            table[4,1] = table[4,0]

            table[5,0] = math.degrees(math.atan(table[1,0]/table[2,0]))
            #print(self.a[1]**2+table[3,1]**2-self.d[3]**2)/(2*self.a[1]*table[3,1])
            table[5,1] = math.degrees(math.acos((self.a[1]**2+table[3,1]**2-self.d[3]**2)/(2*self.a[1]*table[3,1])))

            table[6,0] = math.degrees(math.acos((self.a[1]**2+table[3,0]**2-self.d[3]**2)/(2*self.a[1]*table[3,0])))
            table[6,1] = math.degrees(math.atan(table[2,1]/table[1,0]))

            try:
                table[9,0] = math.degrees(math.atan(self.d[3]/self.a[2]))  
            except:
                table[9,0] = 90

            table[9,1] = table[9,0]

            table[7,0] = 180 - math.degrees(math.acos((table[4,0]**2+self.a[1]**2-table[3,0]**2)/(2*math.fabs(table[4,0])*self.a[1]))) +90-table[9,0]
            table[7,1] = table[7,0]

            table[8,0] = 0
            table[8,1] = 90-table[5,1] -table[6,1]

            table[10,0] = -(table[5,0]+table[6,0])
            table[10,1] = -180 + table[8,1]

            table[11,0] = table[7,0] 
            table[11,1] = table[7,1]

            #select the correct results from the table
            #depends on the configuration of the arm
            #print("table_top ", table[2,0])
            #print("10 0", table[10,0])
            #print("10 1", table[10,1])
            if (table[2,0] < 0):
                J2 = table[10,1]
                J3 = table[11,1]
            else:
                J2 = table[10,0]
                J3 = table[11,0]

        except:
            print("math error, calc_J2J3() - result is garbage")
            J2=-1
            J3=-1
        #print("Table")
        #print(table)
        return J2, J3


    def calc_J456(self, R36, current_joints):

        #print("R36", R36)

        try:
            J5 = math.degrees(math.atan2(math.sqrt(1-R36[2,2]**2),R36[2,2]))
            
            R8Flag = current_joints[4] #1 #simulate seemingly extra value in code change between -1 and 1

            if J5 > 0 and R8Flag > 0:
                J4 = math.degrees(math.atan2(R36[1,2],R36[0,2]))
                #print("here")
                #print(R36[1,2])
                #print(R36[0,2])
                #print(J4)
                
            else:
                J5 = math.degrees(math.atan2(-1*math.sqrt(1-R36[2,2]**2),R36[2,2]))
                J4 = math.degrees(math.atan2(-R36[1,2],-R36[0,2]))
                
                #if (R36[2,1] < 0):
                #    J6 = math.degrees(math.atan2(R36[2,2], -R36[0,2])) + 180
                #else:
                #    J6 = math.degrees(math.atan2(R36[2,1], -R36[2,0])) - 180
            #print(J5)
            
            if J5 < 0:
                if (R36[2,1] < 0):
                    J6 = math.degrees(math.atan2(R36[2,1], -R36[2,0])) + 180
                else:
                    J6 = math.degrees(math.atan2(R36[2,1], -R36[2,0])) - 180
            else:
                #print("here", R36[2,0], R36[2,1])
                if (R36[2,1] < 0):
                    J6 = math.degrees(math.atan2(-R36[2,1], R36[2,0])) - 180
                else:
                    J6 = math.degrees(math.atan2(-R36[2,1], R36[2,0])) + 180

        except:
            print("math error, calc_J2J3() - result is garbage")
            J4 = 0
            J5 = 0
            J6 = 0

        return J4, J5, J6        

    def inverseKinematics(self,pose_in, current_joints):
        #method to convert a desired pose to joint positions     

        #convert the native input angles to a form suitable for the kinematic model
        current_joints = list(self.native2kin_angles(current_joints))

        pose = list(pose_in)
        
        pose[3] = math.radians(pose[3])
        pose[4] = math.radians(pose[4])
        pose[5] = math.radians(pose[5])

        #print(pose)

        R0T_Matrix = self.calculate_R0T_Matrix(pose)
        #print(R0T_Matrix)
        R0T_workFrame_offset = self.workFrame.dot(R0T_Matrix)

        #workout why the negative 1 is needed for this offset to be applied
        R0T_workFrame_offset[0,0] = -1*R0T_workFrame_offset[0,0]
        
        inverted_toolFrame = np.transpose(self.toolFrame)
        #print("inv_toolframe")
        #print(inverted_toolFrame)

        R06 = R0T_workFrame_offset.dot(inverted_toolFrame)

        R06_removal_matrix = self.gen_R06_remove()
        
        #print(R06)
        #print(R06_removal_matrix)
        #wrist centre
        H05 = R06.dot(R06_removal_matrix)
        #print(H05)
        #calculate J1 position
        
        J1 = self.calc_J1(pose, H05)
        #print("J1", J1)
        J2, J3 = self.calc_J2J3(pose, H05)

        J_13 = [J1,J2,(J3-90)]

        A_matrix = self.calculate_A_matrix(J_13)
        #print(A_matrix)
        H01 = self.workFrame.dot(A_matrix[0])
        #print(A_matrix[0])
        H02 = H01.dot(A_matrix[1])
        H03 = H02.dot(A_matrix[2])
#       print("H01 ", H01)
        #transpose the oriention matrix of joint 3 relative to base frame
        R03_transpose = np.transpose(H03[0:3,0:3])
        
        #print("R03 ", list(R03_transpose[2]))
        #not extract the orientation information from H05 (wrist centre)
        R05 = np.array(H05[0:3,0:3])
        #print("R05 0", (R05[0,1]))
        #print("R05 1", (R05[1,1]))
        #print("R05 2", (R05[2,1]))

        #print("dot ", R03_transpose[2,0]*R05[0,1] + R03_transpose[2,1]*R05[1,1]+R03_transpose[2,2]*R05[2,1])
        #find the orientation of the wrist centre relative to J3#
        R36 = R03_transpose.dot(R05)
        #print("R36" , (R36))
        J4, J5, J6 = self.calc_J456(R36, current_joints)

        Joint_angles =  [J1 ,J2 ,J3 ,J4 ,J5 , J6]
        
        result = self.check_limits(Joint_angles)

        #convert the result back so that the robotic arm firmware can use it
        Joint_angles = list(self.kin2native_angles(Joint_angles))
        
        if result == 1:
            return Joint_angles
        else:
            print("No valid solution found")
            return -1#joint configuration is invalid

    def native2kin_angles(self, native_angles):
        
        #map the firmware angles to the kinematic model angles
        
        kin_angles = [0,0,0,0,0,0]

        kin_angles[0] = native_angles[0] - 170
        kin_angles[1] = native_angles[1] - 132
        kin_angles[2] = 140 - (native_angles[2] - 1)
        kin_angles[3] = -1*native_angles[3] + 164
        kin_angles[4] = native_angles[4] - 105
        kin_angles[5] = -1*native_angles[5] + 160

        return kin_angles

    def kin2native_angles(self, kin_angles):
        
        #map the kinematic model angles to the firmware angles

        native_angles = [0,0,0,0,0,0]
        
        native_angles[0] = kin_angles[0] + 170
        native_angles[1] = kin_angles[1] + 132
        native_angles[2] = -1*(kin_angles[2] - 140) + 1
        native_angles[3] = -1*(kin_angles[3] - 164)
        native_angles[4] = kin_angles[4] + 105
        native_angles[5] = -1*(kin_angles[5] - 160)

        return native_angles


    def check_limits(self, joint):
        #check the kinematic angles do not lie outside of a certain range so that the kinematic model does not fail
        #required as there is no backward configuration for the arm in the model
        #array to identify which joints have passed and failed
        test_results = [0,0,0,0,0,0]

        if joint[0] <-170 or joint[0] > 170:
            print("Joint 0: Out of range")
            test_results[0] = 1

        if joint[1] <-132 or joint[1] > 0:
            print("Joint 1: Out of range")
            test_results[1] = 1

        if joint[2] < 1 or joint[2] > 140:
            print("Joint 2: Out of range")
            test_results[2] = 1

        if joint[3] <-164 or joint[3] > 164:
            print("Joint 3: Out of range")
            test_results[3] = 1


        if joint[4] <-105 or joint[4] > 105:
            print("Joint 4: Out of range")
            test_results[4] = 1

        if joint[5] <-160 or joint[5] > 160:
            print("Joint 5: Out of range")
            test_results[5] = 1


        if sum(test_results) > 0:
            result = 0 #failed test
            print("Invalid joint configuration requested")
        else:
            result = 1#passed test

        return result
    




