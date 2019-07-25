#NAME:  start.py
#DATE:  Monday 15th July 2019
#AUTH:  Ryan McCartney
#DESC:  A python script for starting robotic arm utility
#COPY:  Copyright 2019, All Rights Reserved, Ryan McCartney

import os

if __name__ == '__main__':
    
    print("Starting Robotic Arm API")
    cwd = os.getcwd()
    mainPath = cwd+"/main.py"
    os.system("sudo python3 "+mainPath)