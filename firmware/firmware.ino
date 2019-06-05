#include "Joint.h"
#include <Servo.h>

//motors
#define J1Step 2
#define J1Dir 22
#define J1Enable 23
#define J1Inverted false
#define J1StepsPerDegree 44
#define J1SwitchPin 40

#define J1Delay 500
#define J1StartingDelay 10000
#define J1ProfileSteps 300

#define J2Step 3
#define J2Dir 24
#define J2Enable 25
#define J2Inverted false
#define J2StepsPerDegree 60
#define J2SwitchPin 41

#define J2Delay 600
#define J2StartingDelay 10000
#define J2ProfileSteps 300

#define J3Step 4
#define J3Dir 26
#define J3Enable 27
#define J3Inverted false
#define J3StepsPerDegree 60
#define J3SwitchPin 42

#define J3Delay 600
#define J3StartingDelay 10000
#define J3ProfileSteps 300

#define J4Step 5
#define J4Dir 28
#define J4Enable 29
#define J4Inverted false
#define J4StepsPerDegree 44
#define J4SwitchPin 43

#define J4Delay 300
#define J4StartingDelay 10000
#define J4ProfileSteps 300

//other constants
#define headPin 13
#define calibrationSpeed 10

//calibration & continuous movement flags
bool isCalibrated = false;
bool continuousMovement = false;

//motor & switch data
int continuousMovementSpeeds[] = {0,0,0,0,0,0};

//motor & servo objects
#define TOTAL_JOINTS 4
Joint joints[] = {
   Joint(new StepperMotor(J1Step,J1Dir,J1Enable,J1Inverted,J1StepsPerDegree,J1Delay,J1StartingDelay,J1ProfileSteps), J1SwitchPin, 180),
   Joint(new StepperMotor(J2Step,J2Dir,J2Enable,J2Inverted,J2StepsPerDegree,J2Delay,J2StartingDelay,J2ProfileSteps), J2SwitchPin, 180),
   Joint(new StepperMotor(J3Step,J3Dir,J3Enable,J3Inverted,J3StepsPerDegree,J3Delay,J3StartingDelay,J3ProfileSteps), J3SwitchPin, 180),
   Joint(new StepperMotor(J4Step,J4Dir,J4Enable,J4Inverted,J4StepsPerDegree,J4Delay,J4StartingDelay,J4ProfileSteps), J4SwitchPin, 180)
};
Servo hand;
 
void setup() { 
 pinMode(headPin,OUTPUT);
 hand.attach(headPin);
 Serial.begin(115200);
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
    if(joints[jointIndex].position + value >= 0 && joints[jointIndex].position + value <= maxRotation){
          joints[jointIndex].move(value);
    }
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
