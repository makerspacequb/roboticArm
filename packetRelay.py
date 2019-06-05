import socket
from glob import glob
import serial
import threading

#define IP of raspberry pi here
ip = "192.168.137.169"
port = 8008
bufferSize = 512

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((ip,port))

#define arduino baudrate here
baudRate = 115200
verbose = True
ser = None

#buffer of commands to be sent at intervals
queue = list()
sentFirstCommand = False

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
    command = queue.pop(0)
    ser.write(command)
    if verbose:
        print("Sent "+command.decode("utf-8"))

def sendCommands():
    global sentFirstCommand
    while True:
        if len(queue) > 0:
            if not sentFirstCommand:
                sendToSerial()
                sentFirstCommand = True
            else:
                line = ser.readline()
                if len(line) > 0:
                    sendToSerial()
                    line = line.decode("utf-8")
                    print(line[0:len(line)-1])

def fetchPacket():
    #fetch and store all packets in queue
    while True:
        data, addr = sock.recvfrom(bufferSize)
        queue.append(data)

initialiseSerial()

if __name__ == "__main__":
    packetThread = threading.Thread(target=fetchPacket)
    serialThread = threading.Thread(target=sendCommands)
    packetThread.setDaemon(True)
    serialThread.setDaemon(True)
    packetThread.start()
    serialThread.start()
    while True:
        pass