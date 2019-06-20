#ifndef JOINT_H
#define JOINT_H

#include "StepperMotor.h"

class Joint{
  public:

  //setters
  void setSpeed(int speed);
  void setMinSpeed(int speed);
  void setAccelRate(int rate);
  
  bool isCalibrated = false;
  volatile int positionSteps = 0; 
  
  Joint(int jointNumber, int stepPin, int dirPin, int enablePin, int stepsPerDegree, int speed, int minSpeed, 
        int accelRate, bool enableHIGH, int switchPin, int maxRotation, int motorInvert, int switchBufferLen);
  void move(float degrees);
  void moveTo(float targetPosition);
  bool calibrate();
  void update(unsigned long elapsedMicros);
  bool checkLimitSwitch(){ return limitSwitchActivated; };
  void resetLimitSwitch(){ limitSwitchActivated = false; };
  float getPosDegrees(){return positionSteps/stepsPerDegree; };

  private:
  int jointNumber = 0;
  volatile int bufferPos = 0;
  volatile bool switchState = 0;
  volatile int switchBuffer = 1;
  volatile bool claibrating = false;
  int switchPin, maxRotation, stepsPerDegree, motorInvert,minSpeed,switchBufferLen, defaultSpeed;
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
  this->minSpeed = minSpeed;
  this->defaultSpeed = speed;
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
  
  if((switchState == 1)&&(limitSwitchActivated == false)&&(claibrating==false)){
    //First Time Switch Detected Activated
    Serial.print("INFO: Limit switch on joint ");
    Serial.print(jointNumber);
    Serial.println(" activated");
    contMoveFlag = false;
    //Stop movement 
    stepperMotor->move(0);
    //Reset Position
    positionSteps = 0;
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
    if(motorInvert == 1){
      positionSteps += movDir;
    }
    else{
      positionSteps -= movDir;
    }
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

  int steps = degrees*stepsPerDegree;
 
  if(isCalibrated && (((positionSteps + steps) >= 0) && ((positionSteps + steps) <= (maxRotation*stepsPerDegree)))){
    stepperMotor->move(steps);
  }
  else if(!isCalibrated && (((positionSteps + steps) >= 0) && ((positionSteps + steps) <= (maxRotation*stepsPerDegree)))){
    stepperMotor->move(steps);
  }
  else{
    Serial.println("ERROR: Movement out of range.");
    steps = 0;
    stepperMotor->move(steps);
    }
}

void Joint::moveTo(float targetPosition){
  int steps = (targetPosition*stepsPerDegree) - positionSteps;
  if((targetPosition < maxRotation)&&(targetPosition > 0)){
    move(int(targetPosition/stepsPerDegree));
  }
}

bool Joint::calibrate(){
  
  setSpeed(minSpeed);
  int maxSteps = -maxRotation * stepsPerDegree;

  //Invert direction if set in the config
  if(motorInvert == 1){
    maxSteps = -maxSteps;
    }
    
  movDir = maxSteps / abs(maxSteps);
  positionSteps = 0;
  
  if(!limitSwitchActivated){stepperMotor->move(maxSteps);}
  
  while((!limitSwitchActivated) && (abs(positionSteps) < abs(maxSteps))){
    }
  
  stepperMotor->move(0);
  claibrating = true;
  positionSteps = 0;
  setSpeed(defaultSpeed);
  
  if(limitSwitchActivated){
    isCalibrated = true;
    stepperMotor->move(int(-maxSteps/2));
  }
  else{
    isCalibrated = false;
  }

  claibrating = false;
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
