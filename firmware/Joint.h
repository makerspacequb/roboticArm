#ifndef JOINT_H
#define JOINT_H

#include "StepperMotor.h"

class Joint{
  public:
  Joint(StepperMotor* stepperMotor_, int switchPin_, int maxRotation_);
  void move(int degrees);
  void calibrate();
  void update(unsigned long elapsedMicros);

  //setters
  void setSpeed(int speed);
  void setMinSpeed(int speed);
  void setAccelRate(int rate);
  
  bool isCalibrated = false;
  int position = 0; 

  private:
  int switchPin, maxRotation;
  StepperMotor* stepperMotor;
};

Joint::Joint(StepperMotor* stepperMotor, int switchPin, int maxRotation){
	this->stepperMotor = stepperMotor;
	this->switchPin = switchPin;
  this->maxRotation = maxRotation;
	pinMode(switchPin,INPUT);
};

void Joint::update(unsigned long elapsedMicros){
  stepperMotor->step(elapsedMicros);
  //poll switch
}

void Joint::move(int degrees){
  if(isCalibrated && position + degrees >= 0 && position + degrees <= maxRotation){
    position += degrees;
    stepperMotor->moveMotorDegrees(degrees);
  }else if(!isCalibrated){
    stepperMotor->moveMotorDegrees(degrees);
  }
}

void Joint::calibrate(){
  while(!digitalRead(switchPin)){
    stepperMotor->moveMotorDegrees(1);
  }
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
