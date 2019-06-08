#ifndef JOINT_H
#define JOINT_H

#include "StepperMotor.h"

class Joint{
  public:
  Joint(StepperMotor* stepperMotor, int switchPin, int maxRotation);
  void move(int degrees);
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
  int switchPin, maxRotation;
  StepperMotor* stepperMotor;
  volatile bool limitSwitchFlag, contMoveFlag;
  volatile int movDir;
};

Joint::Joint(StepperMotor* stepperMotor, int switchPin, int maxRotation){
  //TODO move stepper creation to this constructor, move stepsPerDegree here, pass
  // stepperMotor speed in steps/ sec not degrees. remove stepsPerDegree from stepperMotor entirely
	this->stepperMotor = stepperMotor;
	this->switchPin = switchPin;
  this->maxRotation = maxRotation;
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
  if (!digitalRead(switchPin) || position >= maxRotation){
    limitSwitchFlag = true;
    contMoveFlag = false;
    // set motor movement to 0
    stepperMotor->moveDegrees(0);
  }
  if (position < 0){
     // TODO need calibration if this happens
  }
}

void Joint::move(int degrees){
  movDir = degrees / abs(degrees);
  //TODO needs fixed
  if(isCalibrated && position + degrees >= 0 && position + degrees <= maxRotation){
    stepperMotor->moveDegrees(degrees);
  } else if(!isCalibrated){
    stepperMotor->moveDegrees(degrees);
  }
}

void Joint::calibrate(int jointNumber){
  stepperMotor->moveDegrees(-maxRotation);
  while(!digitalRead(switchPin)){
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
  stepperMotor->setSpeed(speed);
}

void Joint::setMinSpeed(int speed){
  stepperMotor->setMinSpeed(speed);
}

void Joint::setAccelRate(int rate){
  stepperMotor->setAccelRate(rate);
}

#endif
