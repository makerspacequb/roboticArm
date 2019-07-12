#include "Joint.h"
#include "pins.h"
#include "Config.h"
#include "TimerOne.h"
#include <Servo.h>
#include <EEPROM.h>

//calibration & continuous movement flags
unsigned long statusTime = millis();
const int statusDelay = (1/STATUS_FREQ)*1000;
bool armCalibrated = false;
volatile bool eStopActivated = false;
char instruction[INST_ARRAY_LEN];
int instIndex = 0;

//motor & servo objects
Joint joints[TOTAL_JOINTS] = {
   Joint(J0_STEP, &J0_STEP_PORT, J0_STEP_BYTE, J0_DIR,J0_ENABLE,J0_STEPS_PER_DEG,J0_SPEED,J0_MIN_SPEED,J0_ACCEL_RATE, 
          J0_ENABLE_HIGH, J0_L_SWITCH, &J0_SWITCH_PORT, J0_SWITCH_BYTE, J0_MAX_ROT_DEG, J0_MOTOR_INVERT,J0_DEFAULT_POS),
   Joint(J1_STEP, &J1_STEP_PORT, J1_STEP_BYTE, J1_DIR,J1_ENABLE,J1_STEPS_PER_DEG,J1_SPEED,J1_MIN_SPEED,J1_ACCEL_RATE, 
          J1_ENABLE_HIGH, J1_L_SWITCH, &J1_SWITCH_PORT, J1_SWITCH_BYTE, J1_MAX_ROT_DEG, J1_MOTOR_INVERT,J1_DEFAULT_POS),
   Joint(J2_STEP, &J2_STEP_PORT, J2_STEP_BYTE, J2_DIR,J2_ENABLE,J2_STEPS_PER_DEG,J2_SPEED,J2_MIN_SPEED,J2_ACCEL_RATE, 
          J2_ENABLE_HIGH, J2_L_SWITCH, &J2_SWITCH_PORT, J2_SWITCH_BYTE, J2_MAX_ROT_DEG, J2_MOTOR_INVERT,J2_DEFAULT_POS),
   Joint(J3_STEP, &J3_STEP_PORT, J3_STEP_BYTE, J3_DIR,J3_ENABLE,J3_STEPS_PER_DEG,J3_SPEED,J3_MIN_SPEED,J3_ACCEL_RATE, 
          J3_ENABLE_HIGH, J3_L_SWITCH, &J3_SWITCH_PORT, J3_SWITCH_BYTE, J3_MAX_ROT_DEG, J3_MOTOR_INVERT,J3_DEFAULT_POS),
   Joint(J4_STEP, &J4_STEP_PORT, J4_STEP_BYTE, J4_DIR,J4_ENABLE,J4_STEPS_PER_DEG,J4_SPEED,J4_MIN_SPEED,J4_ACCEL_RATE, 
          J4_ENABLE_HIGH, J4_L_SWITCH, &J4_SWITCH_PORT, J4_SWITCH_BYTE, J4_MAX_ROT_DEG, J4_MOTOR_INVERT,J4_DEFAULT_POS),
   Joint(J5_STEP, &J5_STEP_PORT, J5_STEP_BYTE, J5_DIR,J5_ENABLE,J5_STEPS_PER_DEG,J5_SPEED,J5_MIN_SPEED,J5_ACCEL_RATE, 
          J5_ENABLE_HIGH, J5_L_SWITCH, &J5_SWITCH_PORT, J5_SWITCH_BYTE, J5_MAX_ROT_DEG, J5_MOTOR_INVERT,J5_DEFAULT_POS)
};

Servo hand;

bool interruptBusy = false;
void interrupt(void){
  if(!interruptBusy){
    interruptBusy = true;
    //handle motor movement by interrupt
    for (int i = 0; i < TOTAL_JOINTS; i++)
      joints[i].update(INTERRUPT_TIME);
      interruptBusy = false;
  }
}

void setup() { 
  for (int i = 0; i < TOTAL_JOINTS; i++)
    joints[i].begin();
   
  //Setup Main Serial
  Serial.begin(BAUD_RATE);
  Serial.println("INFO: Starting up...");
  
  //Setup Hand
  pinMode(END_EFFECTOR_2, OUTPUT);
  hand.attach(END_EFFECTOR_2); 

  //Setup Tool Communication line
  Serial1.begin(BAUD_RATE);
  Serial1.println("INFO: Arm Setup Complete.");

  //Load data from EEPROM
  loadPositions();

  //set timer interrupt for motor and encoder control
  Timer1.attachInterrupt(interrupt);
  Timer1.initialize(INTERRUPT_TIME);
  Timer1.start();

  //Set Hardware Interupt for EStop
  pinMode(ESTOP, INPUT);
  attachInterrupt(digitalPinToInterrupt(ESTOP), eStop, FALLING);
  
  Serial.println("INFO: Setup Complete.");
}

void loop() {
  readSerial();
  //Send Status Message at Configured Frequency
  if((millis()-statusTime) > statusDelay){
    sendStatus();
  }
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
      calibrate(input+1);
      break;
    case 'p': 
      moveJointTo(input[1] - '0', atol(input+2));  
      break;
    case 'h': 
      moveHand(atol(input+1)); 
      break;
    case 's': 
      joints[input[1] - '0'].setSpeed(atol(input+2));
      Serial.print("INFO: Set motor: ");
      Serial.print(input[1]);
      Serial.print(" to speed: ");
      Serial.print(atol(input+2));
      Serial.println("deg/s");
      break;
    case 'd': 
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
      quit();
      break;
    case 'r':
      eStopActivated = false;
      Serial.println("INFO: Emergency stop reset.");
      break;
    case 'i':
      //Return information about positions
      printPositions();
      savePositions();
      break;
    case 't':
      //Send Message to Tool
      Serial1.println(input+1);
      //Status output
      Serial.print("TOOL: '");
      Serial.print(input+1);
      Serial.println("' sent to tool on end effector.");
      break;
    default: 
      Serial.println("WARNING: Command not found");
  }
}

void calibrate(char *command){
    
  if(!eStopActivated){  
    //Calibrate Whole Arm
    if(*command == 'a'){
      Serial.println("INFO: Whole arm calibration beginning.");
      for(int i = TOTAL_JOINTS-1; i >= 0; i--){
        joints[i].resetCalibration();
        sendStatus();
        Serial.print("INFO: Calibrating joint ");
        Serial.print(i);
        Serial.println(".");
          
        if(!joints[i].calibrate()){
          Serial.print("ERROR: Calibration of joint ");
          Serial.print(i);
          Serial.println(" failed.");
          }
        else{
          Serial.print("INFO: Calibration of joint ");
          Serial.print(i);
          Serial.println(" complete.");
        }
      }
    }
    //Calibrate an Individual Joint
    else{
      int jointNum = atol(command);
      joints[jointNum].resetCalibration();
      sendStatus();
      Serial.print("INFO: Calibrating joint ");
      Serial.print(jointNum);
      Serial.println(".");

      if(!joints[jointNum].calibrate()){
        Serial.print("ERROR: Calibration of joint ");
        Serial.print(jointNum);
        Serial.println(" failed.");
        }
     else{
        Serial.print("INFO: Calibration of joint ");
        Serial.print(jointNum);
        Serial.println(" complete.");
        }
    }

    //Check Arm Calibration Status
    armCalibrated = true;
    for(int i = TOTAL_JOINTS-1; i >= 0; i--){
        armCalibrated &= joints[i].checkCalibration();
    }
     
    //Return Arm Calibration Status Message
    if(!armCalibrated){
      Serial.println("INFO: Arm not fully calibrated.");
    }
    else{
      Serial.println("INFO: Arm fully calibrated.");
    }  
  }
  else{
    Serial.println("WARNING: Calibration Disabled. Reset with 'r' to continue.");
  }
}

void printMovemetStates(){
  String outputString = "STATUS: MOVEMENT";
  for(int i = 0; i < TOTAL_JOINTS; i++) {
    outputString += ","+(String)(joints[i].checkMovement());
  }
  Serial.println(outputString);
}

void printSwitchStates(){
  String outputString = "STATUS: SWITCH";
  for(int i = 0; i < TOTAL_JOINTS; i++) {
    outputString += ","+(String)(joints[i].checkLimitSwitch());
  }
  Serial.println(outputString);
}

void printPositions(){
  String outputString = "STATUS: POSITION";
  for(int i = 0; i < TOTAL_JOINTS; i++) {
    outputString += ","+(String)(joints[i].getPosDegrees());
  }
  Serial.println(outputString);
}

void printCalibration(){
  String outputString = "STATUS: CALIBRATION";
  for(int i = 0; i < TOTAL_JOINTS; i++) {
    outputString += ","+(String)(joints[i].checkCalibration());
  }
  Serial.println(outputString);
}

void savePositions(){
  for(int i = 0; i < TOTAL_JOINTS; i++) {
    EEPROM.update(i,joints[i].positionSteps);
  }
}

void loadPositions(){
  for(int i = 0; i < TOTAL_JOINTS; i++) {
    joints[i].positionSteps = EEPROM.read(i);
    Serial.print("DATA: Reading position for joint ");
    Serial.print(i);
    Serial.println(" from EEPROM.");
  }
}

void moveHand(int value) {
  if(value > 100){
    Serial.println("ERROR: Invalid hand movement");
  } 
  else{
    int mapping = map(value,0,100,110,180);
    hand.write(mapping);
    delay(15);
    Serial.print("INFO: Moving hand to ");
    Serial.print(value);
    Serial.println("% open");
  }
}

void moveJoint(int jointIndex, int value){
  if(!eStopActivated){
    if(!armCalibrated)
      Serial.println("WARNING: Motors are not calibrated. Calibrate with 'c' command.");
      Serial.print("INFO: Moving motor ");
      Serial.print(jointIndex);
      Serial.print(", ");
      Serial.print(value);
      Serial.println(" degrees");
      joints[jointIndex].move(value);
    }
  else{
    Serial.println("WARNING: Movement Disabled. Reset with 'r' to continue.");
  }
}

void moveJointTo(int jointIndex, int value){
  if(!eStopActivated){
    if(joints[jointIndex].checkCalibration()){
      Serial.print("INFO: Moving motor ");
      Serial.print(jointIndex);
      Serial.print(" to position ");
      Serial.print(value);
      Serial.println(" degrees");
      joints[jointIndex].moveTo(value);
    } 
    else{
      Serial.print("WARNING: Joint ");
      Serial.print(jointIndex);
      Serial.print(" is not calibrated. Calibrate with 'c");
      Serial.print(jointIndex);
      Serial.println("' command.");
    }
  }
  else{
    Serial.println("WARNING: Movement Disabled. Reset with 'r' to continue.");
  }
}

void sendStatus(){
  savePositions();
  printPositions();
  printSwitchStates();
  printCalibration();
  printMovemetStates();
  statusTime = millis();
}

void quit(){
  for (int i = 0; i < TOTAL_JOINTS; i++){
    joints[i].move(0);
  }
  Serial.println("INFO: Arm Stopped");
}

void eStop(){
  for (int i = 0; i < TOTAL_JOINTS; i++){
    joints[i].move(0);
  }
  if(!eStopActivated){
    eStopActivated = true;
    Serial.println("INFO: Emergency Stop Pressed. Release button and reset with 'r' to continue.");
  }
}
