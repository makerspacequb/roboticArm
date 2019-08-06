#NAME:  start.py
#DATE:  Tuesday 6th August 2019
#AUTH:  Ryan McCartney
#DESC:  A python script for starting Robotic Arm API
#COPY:  Copyright 2019, All Rights Reserved, Ryan McCartney

import os

if __name__ == '__main__':
    
    print("Starting Robotic Arm API")
    cwd = os.getcwd()
    mainPath = cwd+"/http_api/main.py"
    #os.system("sudo python3 "+mainPath)