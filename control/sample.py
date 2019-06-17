

        #Check number of gamepads
        gamepads = pygame.joystick.get_count()

        #Log Entry
        currentDateTime = time.strftime("%d/%m/%Y %H:%M:%S")
        logEntry = str(currentDateTime) + ": " + "INFO = ",str(gamepads)," gamepads avalible." + "\n"
        print(logEntry) 

        if gamepads > 0:

            #Initialise first gamepad
            j = pygame.joystick.Joystick(0)
            j.init()
            
            #Check axis avalible
            axis = j.get_numaxes()
            
            #Log Entry
            currentDateTime = time.strftime("%d/%m/%Y %H:%M:%S")
            logEntry = str(currentDateTime) + ": " + "INFO = Gamepad with ",str(axis)," axis has been initiated." + "\n"
            print(logEntry) 

            #Initialsie Speed and Angle
            setSpeed = 0
            setAngle = 0
            payload = str(setSpeed) + "," + str(setAngle) + ",RUN" + "\r\n"
            wheelchair.write(payload)

            while 1:
                
                time.sleep(0.2)

                #Get Current Data
                pygame.event.get()

                xAxisLeft = j.get_axis(0)
                yAxisLeft = j.get_axis(1)
                aButton = j.get_button(0)
                bButton = j.get_button(1)
                yButton = j.get_button(2)
                xButton = j.get_button(3)

                #print("Raw data =",xAxisLeft,",",yAxisLeft)

                #Mapped Data for API 
                speed = int(-yAxisLeft*topSpeed)
                angle = int(-xAxisLeft*100)

                #On button presses start and stop wheelchair
                if aButton == True:
                    wheelchair.write("0,0,RESET\r\n".encode())
                    print("RESET Command Sent")
                if bButton == True:
                    wheelchair.write("0,0,STOP\r\n".encode())
                    print("STOP Command Sent")
                if xButton == True:
                    topSpeed = topSpeed + 1
                    if topSpeed > 100:
                        topSpeed = 100
                    print("INFO: Top Speed is now",topSpeed)
                if yButton == True:
                    topSpeed = topSpeed - 1
                    if topSpeed < 0:
                        topSpeed = 0
                    print("INFO: Top Speed is now",topSpeed)
               
                #If new command has been identified then send new data to API
                if (setSpeed != speed) or (setAngle != angle):
                    
                    payload = str(speed) + "," + str(angle) + ",RUN" + "\r\n"
                    wheelchair.write(payload.encode())

                    setSpeed = speed
                    setAngle = angle
                    print("Mapped speed is",speed,"and the angle is",angle)

                while wheelchair.in_waiting:
                    info = wheelchair.readline()
                    print(info)