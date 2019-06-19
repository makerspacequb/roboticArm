#include "Joint.h"
#include "pins.h"
#include "Config.h"
#include "TimerOne.h"
#include <Servo.h>

//calibration & continuous movement flags
bool armCalibrated = false;
bool eStopActivated = false;
char instruction[INST_ARRAY_LEN];
int instIndex = 0;

//motor & servo objects
Joint joints[TOTAL_JOINTS] = {
   Joint(J0_STEP,J0_DIR,J0_ENABLE,J0_STEPS_PER_DEG,J0_SPEED,J0_MIN_SPEED,J0_ACCEL_RATE, 
          J0_ENABLE_HIGH, J0_L_SWITCH, J0_MAX_ROT_DEG,J0_MOTOR_INVERT),
   Joint(J1_STEP,J1_DIR,J1_ENABLE,J1_STEPS_PER_DEG,J1_SPEED,J1_MIN_SPEED,J1_ACCEL_RATE, 
          J1_ENABLE_HIGH, J1_L_SWITCH, J1_MAX_ROT_DEG,J1_MOTOR_INVERT),
   Joint(J2_STEP,J2_DIR,J2_ENABLE,J2_STEPS_PER_DEG,J2_SPEED,J2_MIN_SPEED,J2_ACCEL_RATE, 
          J2_ENABLE_HIGH, J2_L_SWITCH, J2_MAX_ROT_DEG,J2_MOTOR_INVERT),
   Joint(J3_STEP,J3_DIR,J3_ENABLE,J3_STEPS_PER_DEG,J3_SPEED,J3_MIN_SPEED,J3_ACCEL_RATE, 
          J3_ENABLE_HIGH, J3_L_SWITCH, J3_MAX_ROT_DEG,J3_MOTOR_INVERT),
   Joint(J4_STEP,J4_DIR,J4_ENABLE,J4_STEPS_PER_DEG,J4_SPEED,J4_MIN_SPEED,J4_ACCEL_RATE, 
          J4_ENABLE_HIGH, J4_L_SWITCH, J4_MAX_ROT_DEG,J4_MOTOR_INVERT),
   Joint(J5_STEP,J5_DIR,J5_ENABLE,J5_STEPS_PER_DEG,J5_SPEED,J5_MIN_SPEED,J5_ACCEL_RATE, 
          J5_ENABLE_HIGH, J5_L_SWITCH, J5_MAX_ROT_DEG,J5_MOTOR_INVERT)
};
Servo hand;

bool interruptBusy = false;
void interrupt(void){
  if(!interruptBusy){
    interruptBusy = true;
    //handle motor movement by interrupt
    //step motors
    for (int i = 0; i < TOTAL_JOINTS; i++){
      joints[i].update(INTERRUPT_TIME);
    }
    //Send Motor Positions for status Message
    printPositions();
    interruptBusy = false;
  }
}

void setup() { 
  //Setup Hand
  pinMode(END_EFFECTOR_2,OUTPUT);
  hand.attach(END_EFFECTOR_2);
  moveHand(50); 

  Serial.begin(BAUD_RATE);

  //Setup Tool Communication line
  Serial1.begin(BAUD_RATE);
  Serial1.println("INFO: Arm Setup Complete.");

  for(int i = 38; i < 50; i=i+2) {
    pinMode(i,INPUT_PULLUP);
  }

  //Set Hardware Interupt for EStop
  attachInterrupt(digitalPinToInterrupt(ESTOP), eStop, FALLING);
  
  //set timer interrupt for motor and encoder control
  Timer1.attachInterrupt(interrupt);
  Timer1.initialize(INTERRUPT_TIME);
  Timer1.start();
  Serial.println("INFO: Setup Complete.");
}

void loop() {
  readSerial();
}

void readSerial(){
  if (Serial.available() > 0){
    char nextChar = Serial.read();
    //if char is new line last instruction complete, process instruction
    if(nextChar == '\n'){
      if(instIndex > 0) {
        //make sure rest of instruction is cleared
        for (int i = instIndex; i < INST_ARRAY_LEN; i++)
          instruction[i] = NULL;
        //send instruction for processing
        processInstruction(instruction);
        instIndex = 0;
      }
    }
    //add to instruction string
    else {
      if(instIndex >= INST_ARRAY_LEN)
        Serial.println("ERROR: Instruction parser: Instruction index out of bounds.");
      else{
        instruction[instIndex] = nextChar;
        instIndex++;
      }
    }
  }
}

void processInstruction(char *input){
  //check first byte
  switch(toLowerCase(input[0])){
    case 'c': 
      calibration(); 
      break;
    case 'p': 
      moveJointTo(input[1] - '0',atol(input+2));  
      break;
    case 'h': 
      moveHand(atol(input+1)); 
      break;
    case 'x': 
      //continuousMovement = atol(input+1);
      break;
    case 'd': 
      joints[input[1] - '0'].setSpeed(atol(input+2));
      Serial.print("INFO: Set motor: ");
      Serial.print(input[1]);
      Serial.print(" to speed: ");
      Serial.print(atol(input+2));
      Serial.println("deg/s");
      break;
    case 's': 
      joints[input[1] - '0'].setMinSpeed(atol(input+2));
      Serial.print("INFO: Set motor: ");
      Serial.print(input[1]);
      Serial.print(" min speed to: ");
      Serial.print(atol(input+2));
      Serial.println("deg/s");
      break;
    case 'z': 
      joints[input[1] - '0'].setAccelRate(atol(input+2));
      Serial.print("INFO: Set motor: ");
      Serial.print(input[1]);
      Serial.print(" acceleration steps: ");
      Serial.print(atol(input+2));
      Serial.println("us/step");
      break;
    case 'm': 
      moveJoint(input[1] - '0',atol(input+2)); 
      break;
    case 'q': 
      for (int i = 0; i < TOTAL_JOINTS; i++)
        joints[i].move(0);
      Serial.println("INFO: Arm Stopped");
      break;
    case 'r':
      eStopActivated = false;
      Serial.println("INFO: Emergency stop reset.");
      break;
    case 't':
      //Send Message to Tool
      String toolMessage = input+10;
      Serial1.println(toolMessage);
      //Status output
      Serial.print("TOOL: '");
      Serial.print(toolMessage);
      Serial.print("' sent to tool on end effector.");
      break;
    default: 
      Serial.println("WARNING: Command not found");
  }
}

void calibration(){
  armCalibrated = true;
  bool isCalibrated = false;
  
  for(int i = TOTAL_JOINTS; i >= 0; i--){
    Serial.print("INFO: Calibrating joint: ");
    Serial.println(i);
    isCalibrated = joints[i].calibrate();
    
    if(!isCalibrated){
      Serial.print("ERROR: Calibration of joint ");
      Serial.print(i);
      Serial.println(" failed");
      }
     //Overall calibration for arm
    armCalibrated = armCalibrated && isCalibrated;
  }
  if(!armCalibrated){
    Serial.println("ERROR: Arm Calibration Failed");
    }
  else{
    Serial.println("INFO: Arm Calibration Succesful");
    }
  
}

void printPositions(){
  String outputString = "STATUS:";
  for(int i = 0; i < TOTAL_JOINTS; i++) {
    outputString += (String)(joints[i].position)+",";
  }
  Serial.println(outputString);
}

void moveHand(int value) {
  if(value > 100){
    Serial.println("ERROR: Invalid hand movement");
  } 
  else{
    int mapping = map(value,0,100,1,120);
    hand.write(mapping);
    delay(15);
    Serial.print("INFO: Moving hand to ");
    Serial.print(value);
    Serial.println("% open");
  }
}

void moveJoint(int jointIndex, int value){
  if(!eStopActivated){
    if(armCalibrated){
      Serial.print("INFO: Moving motor ");
      Serial.print(jointIndex);
      Serial.print(", ");
      Serial.print(value);
      Serial.println(" degrees");
      joints[jointIndex].move(value);
    } 
    else{
      Serial.println("WARNING: Motors are not calibrated. Calibrate with 'c' command.");
      joints[jointIndex].move(value);
    }
  }
  else{
    Serial.println("WARNING: EStop activated. Reset with 'r' to continue.");
  }
}

void moveJointTo(int jointIndex, int value){
  if(!eStopActivated){
    if(armCalibrated){
      Serial.print("INFO: Moving motor ");
      Serial.print(jointIndex);
      Serial.print(" to position ");
      Serial.print(value);
      Serial.println(" degrees");
      joints[jointIndex].moveTo(value);
    } 
    else{
      Serial.println("WARNING: Motors are not calibrated. Calibrate with 'c' command.");
      joints[jointIndex].move(value);
    }
  }
  else{
    Serial.println("WARNING: EStop activated. Reset with 'r' to continue.");
  }
}

void eStop(){
  for (int i = 0; i < TOTAL_JOINTS; i++){
    joints[i].move(0);
  }
  if(!eStopActivated){
    Serial.println("INFO: Emergency Stop Pressed. Release button and reset with 'r' to continue.");
    eStopActivated = true;
  }
}
