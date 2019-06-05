#include "Joint.h"
#include "pins.h"
#include "Config.h"
#include <Servo.h>

//calibration & continuous movement flags
bool isCalibrated = false;
bool continuousMovement = false;

//motor & switch data
int continuousMovementSpeeds[] = {0,0,0,0,0,0};

//motor & servo objects
Joint joints[TOTAL_JOINTS] = {
   Joint(new StepperMotor(J0_STEP,J0_DIR,J0_ENABLE,J0_STEPS_PER_DEG,J0_DELAY,J0_START_DELAY,J0_PROFILE_STEPS), J0_L_SWITCH, J0_MAX_ROT_DEG),
   Joint(new StepperMotor(J1_STEP,J1_DIR,J1_ENABLE,J1_STEPS_PER_DEG,J1_DELAY,J1_START_DELAY,J1_PROFILE_STEPS), J1_L_SWITCH, J1_MAX_ROT_DEG),
   Joint(new StepperMotor(J2_STEP,J2_DIR,J2_ENABLE,J2_STEPS_PER_DEG,J2_DELAY,J2_START_DELAY,J2_PROFILE_STEPS), J2_L_SWITCH, J2_MAX_ROT_DEG),
   Joint(new StepperMotor(J3_STEP,J3_DIR,J3_ENABLE,J3_STEPS_PER_DEG,J3_DELAY,J3_START_DELAY,J3_PROFILE_STEPS), J3_L_SWITCH, J3_MAX_ROT_DEG)
};
Servo hand;
 
void setup() { 
 pinMode(HAND_PIN,OUTPUT);
 hand.attach(HAND_PIN);
 Serial.begin(BAUD_RATE);
}

void loop() {
  //printSwitches();
  if(Serial.available()){
   String cmd = Serial.readString();
   char firstChar = toLowerCase(cmd.charAt(0));
   switch(firstChar){
    case 'c': calibration(); break;
    case 'p': printPositions(); break;
    case 'h': moveHand(cmd.substring(1).toInt()); break;
    case 'x': continuousMovement = (bool)cmd.substring(1).toInt(); break;
    case 'd': setJointDelay((int)(cmd.charAt(1) - '0'),cmd.substring(2).toInt()); break;
    case 's': setJointStartingDelay((int)(cmd.charAt(1) - '0'),cmd.substring(2).toInt()); break;
    case 'z': setJointProfileSteps((int)(cmd.charAt(1) - '0'),cmd.substring(2).toInt()); break;
    case 'm': moveJoint((int)(cmd.charAt(1) - '0'),cmd.substring(2).toInt()); break;
    default: Serial.println("Command not found");
   }
   printPositions();
 }
 if(continuousMovement){
  for(int i = 0; i < 6; i++){
    moveJoint(i,continuousMovementSpeeds[i]);
  }
 }
}

void calibration(){
  for(int i = 0; i < 1; i++){
    joints[i].calibrate();
    Serial.println("Calibrated "+(String)i);
    delay(100);
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

/*void printSwitches(){
  String outputString = "";
  for(int i = 40; i < 46; i++){
    outputString += (String)digitalRead(i)+",";
  }
  Serial.println(outputString);
}*/

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

void setJointDelay(int jointIndex, int value){
  joints[jointIndex].setDelay(value);
}

void setJointSpeed(int jointIndex, int value){
  continuousMovementSpeeds[jointIndex] = value;
}

void setJointStartingDelay(int jointIndex, int value){
  joints[jointIndex].setStartingDelay(value);
}

void setJointProfileSteps(int jointIndex, int value){
  joints[jointIndex].setProfileSteps(value);
}
