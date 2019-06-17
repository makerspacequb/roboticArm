#ifndef JOINT_H
#define JOINT_H

#include "StepperMotor.h"

class Joint{
  public:
  Joint(int stepPin, int dirPin, int enablePin, int stepsPerDegree, int speed, int minSpeed, 
        int accelRate, int switchPin, int maxRotation);
  void move(float degrees);
  void calibrate(int jointNumber);
  void update(unsigned long elapsedMicros);
  bool checkLimitSwitch(){ return limitSwitchFlag; };
  void resetLimitSwitch(){ limitSwitchFlag = false; };

  //setters
  void setSpeed(int speed);
  void setMinSpeed(int speed);
  void setAccelRate(int rate);
  
  bool isCalibrated = false;
  volatile int positionSteps = 0; 
  int position = 0;

  private:
  int switchPin, maxRotation, stepsPerDegree;
  StepperMotor* stepperMotor;
  volatile bool limitSwitchFlag, contMoveFlag;
  volatile int movDir;
};

Joint::Joint(int stepPin, int dirPin, int enablePin, int stepsPerDegree, int speed, int minSpeed, 
             int accelRate, int switchPin, int maxRotation){
  int speedStepsPerSec = speed * stepsPerDegree;
	stepperMotor = new StepperMotor(stepPin, dirPin, enablePin, speedStepsPerSec, minSpeed, accelRate);
	this->switchPin = switchPin;
  this->maxRotation = maxRotation;
  this->stepsPerDegree = stepsPerDegree;
	pinMode(switchPin,INPUT_PULLUP);
  contMoveFlag = false;
  limitSwitchFlag = false;
  movDir = 1;
};

void Joint::update(unsigned long elapsedMicros){
  if (!limitSwitchFlag)
    if (stepperMotor->step(elapsedMicros, contMoveFlag))
      positionSteps += movDir;
  //poll switch
  if (digitalRead(switchPin) || position >= maxRotation){
    limitSwitchFlag = true;
    contMoveFlag = false;
    // set motor movement to 0
    stepperMotor->move(0);
  }
  if (position < 0){
     // TODO need calibration if this happens
  }
}

void Joint::move(float degrees){
  movDir = degrees / abs(degrees);
  //TODO needs fixed
  if(isCalibrated && position + degrees >= 0 && position + degrees <= maxRotation){
    stepperMotor->move(degrees * stepsPerDegree);
  } else if(!isCalibrated){
    stepperMotor->move(degrees * stepsPerDegree);
  }
}

void Joint::calibrate(int jointNumber){
  stepperMotor->move(-maxRotation * stepsPerDegree);
  while(digitalRead(switchPin)){
    Serial.print("Calibrating joint: ");
    Serial.println(jointNumber);
    delay(1000);
  }
  position = 0;
  Serial.print("Finished calibrating joint: ");
  Serial.println(jointNumber);
  isCalibrated = true;
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
