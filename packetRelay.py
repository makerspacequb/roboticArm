import socket
from glob import glob
from time import time
import serial
import threading

#define IP of raspberry pi here
ip = "192.168.137.169"
port = 8008
bufferSize = 128

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((ip,port))

#define arduino baudrate here
baudRate = 115200
verbose = True
ser = None

#buffer of commands to be sent at intervals
queue = list()
lastSent = 0
#ms delay between serial messages being sent
delay = 10

def initialiseSerial():
    global ser
    ports = glob('/dev/tty[A-Za-z]*')
    print(ports)
    #tries to connect to serial port 
    #(should always be aruduino if only one USB connection)
    for port in ports:
        try:
            print("Trying port "+port)
            ser = serial.Serial(port, baudRate)
            print("Connected")
            break
        except:
            print("Couldn't connect to "+port)
            pass

def sendToSerial():
    global lastSent,delay
    while True:
        #send command to serial every delay ms (if queue not empty)
        if lastSent+delay < time()*1000 and len(queue) > 0:
 	    lastSent = time()*1000
            command = queue.pop(0)
            ser.write(command)
            if verbose:
                print("Sent "+command.decode("utf-8"))

def fetchPacket():
    #fetch and store all packets in queue
    while True:
        data, addr = sock.recvfrom(bufferSize)
        queue.append(data)

initialiseSerial()

if __name__ == "__main__":
    packetThread = threading.Thread(target=fetchPacket)
    serialThread = threading.Thread(target=sendToSerial)
    packetThread.setDaemon(True)
    serialThread.setDaemon(True)
    packetThread.start()
    serialThread.start()
    while True:
        pass