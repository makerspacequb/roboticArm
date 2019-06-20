#ifndef JOINT_H
#define JOINT_H

#include "StepperMotor.h"

class Joint{
  public:
  Joint(int jointNumber, int stepPin, int dirPin, int enablePin, int stepsPerDegree, int speed, int minSpeed, 
        int accelRate, bool enableHIGH, int switchPin, int maxRotation, int motorInvert, int switchBufferLen);
  void move(float degrees);
  void moveTo(float targetPosition);
  bool calibrate();
  void update(unsigned long elapsedMicros);
  bool checkLimitSwitch(){ return limitSwitchActivated; };
  void resetLimitSwitch(){ limitSwitchActivated = false; };

  //setters
  void setSpeed(int speed);
  void setMinSpeed(int speed);
  void setAccelRate(int rate);
  
  bool isCalibrated = false;
  volatile int positionSteps = 0; 
  int position = 0;

  private:
  int jointNumber = 0;
  volatile int bufferPos = 0;
  volatile bool switchState = 0;
  volatile int switchBuffer = 1;
  int switchPin, maxRotation, stepsPerDegree, motorInvert,switchBufferLen;
  StepperMotor* stepperMotor;
  volatile bool limitSwitchActivated, contMoveFlag;
  volatile int movDir;
};

Joint::Joint(int jointNumber, int stepPin, int dirPin, int enablePin, int stepsPerDegree, int speed, int minSpeed, 
             int accelRate, bool enableHIGH, int switchPin, int maxRotation, int motorInvert,int switchBufferLen){
  int speedStepsPerSec = speed * stepsPerDegree;
	stepperMotor = new StepperMotor(stepPin, dirPin, enablePin, speedStepsPerSec, minSpeed, accelRate, enableHIGH);
	this->jointNumber = jointNumber;
	this->switchPin = switchPin;
  this->maxRotation = maxRotation;
  this->stepsPerDegree = stepsPerDegree;
  this->motorInvert = motorInvert;
  this->switchBufferLen = switchBufferLen;
	pinMode(switchPin,INPUT_PULLUP);
  contMoveFlag = false;
  limitSwitchActivated = false;
  movDir = 1;
};

void Joint::update(unsigned long elapsedMicros){
  
  //Poll switch
  switchBuffer &= !digitalRead(switchPin);
  bufferPos++;

  //Check if switch debounce buffer needs reset
  if(bufferPos >= switchBufferLen){
    switchState = switchBuffer;
    switchBuffer = 1;
    bufferPos = 0;
  }
  
  if((switchState == 1)&&(limitSwitchActivated == false)){
    //First Time Switch Detected Activated
    Serial.print("INFO: Limit switch on joint ");
    Serial.print(jointNumber);
    Serial.println(" activated");
    contMoveFlag = false;
    //Stop movement 
    stepperMotor->move(0);
    //Reset Position
    position = 0;
    limitSwitchActivated = true;
  }
  else if(switchState == 1){
    //For every other detections
    limitSwitchActivated = true;
  }
  else{
    //If not detected
    limitSwitchActivated = false;
  }
  
  //Track motor position in Steps
  if (stepperMotor->step(elapsedMicros, contMoveFlag)){
    positionSteps += movDir;
    position = int(positionSteps/stepsPerDegree);
    }

  if (position < 0){
    contMoveFlag = false;
    //Stop movement 
    stepperMotor->move(0);
  }
  
}

void Joint::move(float degrees){
  
  //Invert direction if set in the config
  if(motorInvert == 1){
    degrees = -degrees;
    }
  
  movDir = degrees / abs(degrees);

  //Prevent movement further than the limit switch
  if((limitSwitchActivated == true) && (movDir == -1)){
    degrees = 0;
  }
 
  if(isCalibrated && position + degrees >= 0 && position + degrees <= maxRotation){
    stepperMotor->move(degrees * stepsPerDegree);
  } else if(!isCalibrated){
    stepperMotor->move(degrees * stepsPerDegree);
  }
}

void Joint::moveTo(float targetPosition){
  float degrees = targetPosition - position;
  if((targetPosition < maxRotation)&&(targetPosition > 0)){
    move(degrees);
  }
}


bool Joint::calibrate(){

  int maxMovement = maxRotation * stepsPerDegree;
  
  //Invert direction if set in the config
  if(motorInvert == 1){
    maxMovement = -maxMovement;
    }
    
  stepperMotor->move(maxMovement);
  
  while(!limitSwitchActivated && (maxRotation) ){
    }

  if(limitSwitchActivated){
    position = 0;
    isCalibrated = true;
  }
  else{
    isCalibrated = false;
  }

  return isCalibrated;
}

//setters
void Joint::setSpeed(int speed){
  stepperMotor->setSpeed(speed * stepsPerDegree);
}

void Joint::setMinSpeed(int speed){
  stepperMotor->setMinSpeed(speed * stepsPerDegree);
}

void Joint::setAccelRate(int rate){
  stepperMotor->setAccelRate(rate);
}

#endif
