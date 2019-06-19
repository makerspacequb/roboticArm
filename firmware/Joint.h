#ifndef JOINT_H
#define JOINT_H

#include "StepperMotor.h"

class Joint{
  public:
  Joint(int stepPin, int dirPin, int enablePin, int stepsPerDegree, int speed, int minSpeed, 
        int accelRate, bool enableHIGH, int switchPin, int maxRotation, int motorInvert);
  void move(float degrees);
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
  bool switchStateCurrent = true;
  bool switchStatePrevious = true;
  int switchPin, maxRotation, stepsPerDegree, motorInvert;
  StepperMotor* stepperMotor;
  volatile bool limitSwitchActivated, contMoveFlag;
  volatile int movDir;
};

Joint::Joint(int stepPin, int dirPin, int enablePin, int stepsPerDegree, int speed, int minSpeed, 
             int accelRate, bool enableHIGH, int switchPin, int maxRotation, int motorInvert){
  int speedStepsPerSec = speed * stepsPerDegree;
	stepperMotor = new StepperMotor(stepPin, dirPin, enablePin, speedStepsPerSec, minSpeed, accelRate, enableHIGH);
	this->switchPin = switchPin;
  this->maxRotation = maxRotation;
  this->stepsPerDegree = stepsPerDegree;
  this->motorInvert = motorInvert;
	pinMode(switchPin,INPUT_PULLUP);
  contMoveFlag = false;
  limitSwitchActivated = false;
  movDir = 1;
};

void Joint::update(unsigned long elapsedMicros){
  //Poll switch
  switchStateCurrent = digitalRead(switchPin);
  if((switchStateCurrent == false) && (switchStatePrevious == false)){
    //First time stop movement
    if(limitSwitchActivated == false){
      contMoveFlag = false;
      //Stop movement 
      stepperMotor->move(0);
    }
    limitSwitchActivated = true;
  }
  else{
    limitSwitchActivated = false;
  }
  //Set switch flag
  switchStatePrevious = switchStateCurrent;
  
  if (stepperMotor->step(elapsedMicros, contMoveFlag)){
    positionSteps += movDir;
    }

    
  if (position < 0){
     // TODO need calibration if this happens
  }
  
}

void Joint::move(float degrees){
  movDir = degrees / abs(degrees);

  //Invert direction if set in the config
  if(motorInvert == 1){
    movDir = -movDir;
    }

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

bool Joint::calibrate(){
  
  stepperMotor->move(-maxRotation * stepsPerDegree);
  
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
