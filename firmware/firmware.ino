#include "Joint.h"
#include "pins.h"
#include "Config.h"
#include "TimerOne.h"
#include <Servo.h>

//calibration & continuous movement flags
bool isCalibrated = false;
bool continuousMovement = false;
char instruction[INST_ARRAY_LEN];
int instIndex = 0;

//motor & switch data
int continuousMovementSpeeds[] = {0,0,0,0,0,0};

//motor & servo objects
Joint joints[TOTAL_JOINTS] = {
   Joint(new StepperMotor(J0_STEP,J0_DIR,J0_ENABLE,J0_STEPS_PER_DEG,J0_SPEED,J0_MIN_SPEED,J0_ACCEL_RATE), J0_L_SWITCH, J0_MAX_ROT_DEG),
   Joint(new StepperMotor(J1_STEP,J1_DIR,J1_ENABLE,J1_STEPS_PER_DEG,J1_SPEED,J1_MIN_SPEED,J1_ACCEL_RATE), J1_L_SWITCH, J1_MAX_ROT_DEG),
   Joint(new StepperMotor(J2_STEP,J2_DIR,J2_ENABLE,J2_STEPS_PER_DEG,J2_SPEED,J2_MIN_SPEED,J2_ACCEL_RATE), J2_L_SWITCH, J2_MAX_ROT_DEG),
   Joint(new StepperMotor(J3_STEP,J3_DIR,J3_ENABLE,J3_STEPS_PER_DEG,J3_SPEED,J3_MIN_SPEED,J3_ACCEL_RATE), J3_L_SWITCH, J3_MAX_ROT_DEG),
   Joint(new StepperMotor(J4_STEP,J4_DIR,J4_ENABLE,J4_STEPS_PER_DEG,J4_SPEED,J4_MIN_SPEED,J4_ACCEL_RATE), J4_L_SWITCH, J4_MAX_ROT_DEG),
   Joint(new StepperMotor(J5_STEP,J5_DIR,J5_ENABLE,J5_STEPS_PER_DEG,J5_SPEED,J5_MIN_SPEED,J5_ACCEL_RATE), J5_L_SWITCH, J5_MAX_ROT_DEG)
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
    interruptBusy = false;
  }
}

void setup() { 
  pinMode(HAND_PIN,OUTPUT);
  hand.attach(HAND_PIN);
  Serial.begin(BAUD_RATE);
 
  //set timer interrupt for motor and encoder control
  Timer1.attachInterrupt(interrupt);
  Timer1.initialize(INTERRUPT_TIME);
  Timer1.start();
}

void loop() {
  //printSwitches();
  readSerial();
  
  if(continuousMovement){
    for(int i = 0; i < 6; i++){
      moveJoint(i,continuousMovementSpeeds[i]);
    }
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
        Serial.println("ERROR - loop() instruction parser: Instruction index out of bounds.");
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
      printPositions(); 
      break;
    case 'h': 
      moveHand(atol(input+1)); 
      break;
    case 'x': 
      //continuousMovement = atol(input+1);
      break;
    case 'd': 
      joints[input[1] - '0'].setSpeed(atol(input+2));
      Serial.print("Set motor: ");
      Serial.print(input[1]);
      Serial.print(" to speed: ");
      Serial.print(atol(input+2));
      Serial.println("deg/s");
      break;
    case 's': 
      joints[input[1] - '0'].setMinSpeed(atol(input+2));
      Serial.print("Set motor: ");
      Serial.print(input[1]);
      Serial.print(" min speed to: ");
      Serial.print(atol(input+2));
      Serial.println("deg/s");
      break;
    case 'z': 
      joints[input[1] - '0'].setAccelRate(atol(input+2));
      Serial.print("Set motor: ");
      Serial.print(input[1]);
      Serial.print(" acceleration steps: ");
      Serial.print(atol(input+2));
      Serial.println("us/step");
      break;
    case 'm': 
      moveJoint(input[1] - '0',atol(input+2)); 
      Serial.print("Moving motor: ");
      Serial.print(input[1]);
      Serial.print(" : ");
      Serial.print(atol(input+2));
      Serial.println("degs");
      break;
    case 'q': 
      for (int i = 0; i < TOTAL_JOINTS; i++)
        joints[i].move(0);
      break;
    default: 
      Serial.println("Command not found");
  }
  //printPositions();
}

void calibration(){
  for(int i = 0; i < 1; i++){
    joints[i].calibrate(i);
  }
  isCalibrated = true;
}

void printPositions(){
  String outputString = "";
  for(int i = 0; i < TOTAL_JOINTS; i++) {
    outputString += (String)(joints[i].position)+",";
  }
  Serial.println(outputString);
}

void moveHand(int value) {
  if(value > 90){
    Serial.println("Invalid hand movement");
  } else{
    int mapping = map(value,0,90,0,130);
    hand.write(mapping);
    delay(15);
  }
}

void moveJoint(int jointIndex, int value){
  if(isCalibrated){
    joints[jointIndex].move(value);
  } else{
    //Serial.println("Warning: Motors are not calibrated");
    joints[jointIndex].move(value);
  }
}

