#include "Joint.h"
#include <Servo.h>

//motors
#define J1Pulse 2
#define J1Dir 22
#define J1Enable 23
#define J1Inverted false
#define J1StepsPerDegree 44
#define J1SwitchPin 40

#define J2Pulse 3
#define J2Dir 24
#define J2Enable 25
#define J2Inverted false
#define J2StepsPerDegree 60
#define J2SwitchPin 41

#define J3Pulse 4
#define J3Dir 26
#define J3Enable 27
#define J3Inverted false
#define J3StepsPerDegree 60
#define J3SwitchPin 42

#define J4Pulse 5
#define J4Dir 28
#define J4Enable 29
#define J4Inverted false
#define J4StepsPerDegree 44
#define J4SwitchPin 43

//other constants
#define headPin 13
#define maxRotation 180
#define calibrationSpeed 10

//calibration & continuous movement flags
bool isCalibrated = false;
bool continuousMovement = false;

//motor & switch data
int continuousMovementSpeeds[] = {0,0,0,0,0,0};

//motor & servo objects
Joint joints[] = {
   Joint(new StepperMotor(J1Pulse,J1Dir,J1Enable,J1Inverted,500,J1StepsPerDegree), J1SwitchPin),
   Joint(new StepperMotor(J2Pulse,J2Dir,J2Enable,J2Inverted,600,J2StepsPerDegree), J2SwitchPin),
   Joint(new StepperMotor(J3Pulse,J3Dir,J3Enable,J3Inverted,600,J3StepsPerDegree), J3SwitchPin),
   Joint(new StepperMotor(J4Pulse,J4Dir,J4Enable,J4Inverted,600,J4StepsPerDegree), J4SwitchPin)
};
Servo hand;
 
void setup() { 
 pinMode(headPin,OUTPUT);
 hand.attach(headPin);
 Serial.begin(9600);
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
    case 'm': moveJoint((int)(cmd.charAt(1) - '0'),cmd.substring(2).toInt()); break;
    default: Serial.println("Command not found");
   }
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
  for(int i = 0; i < 6; i++) {
    outputString += (String)joints[i].position+",";
  }
  Serial.println(outputString);
}

void printSwitches(){
  String outputString = "";
  for(int i = 40; i < 46; i++){
    outputString += (String)digitalRead(i)+",";
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

void moveJoint(int motorIndex, int value){
  if(isCalibrated){
    if(joints[motorIndex].position + value >= 0 && joints[motorIndex].position + value <= maxRotation){
          joints[motorIndex].move(value);
    }
  } else{
    //Serial.println("Warning: Motors are not calibrated");
    joints[motorIndex].move(value);
  }
}

void setJointDelay(int motorIndex, int value){
  joints[motorIndex].setDelay(value);
}

void setJointSpeed(int motorIndex, int value){
  continuousMovementSpeeds[motorIndex] = value;
}
