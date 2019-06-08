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
  int position = 0; 

  private:
  int switchPin, maxRotation;
  StepperMotor* stepperMotor;
  volatile bool limitSwitchFlag = false;
};

Joint::Joint(StepperMotor* stepperMotor, int switchPin, int maxRotation){
	this->stepperMotor = stepperMotor;
	this->switchPin = switchPin;
  this->maxRotation = maxRotation;
	pinMode(switchPin,INPUT_PULLUP);
};

void Joint::update(unsigned long elapsedMicros){
  if (!limitSwitchFlag)
    stepperMotor->step(elapsedMicros);
  //poll switch
  if (!digitalRead(switchPin)){
    limitSwitchFlag = true;
    // set motor movement to 0
    stepperMotor->moveMotorDegrees(0);
  }
}

void Joint::move(int degrees){
  if(isCalibrated && position + degrees >= 0 && position + degrees <= maxRotation){
    position += degrees;
    stepperMotor->moveMotorDegrees(degrees);
  }else if(!isCalibrated){
    stepperMotor->moveMotorDegrees(degrees);
  }
}

void Joint::calibrate(int jointNumber){
  stepperMotor->moveMotorDegrees(-maxRotation);
  while(!digitalRead(switchPin)){
    Serial.print("Calibrating joint: ");
    Serial.println(jointNumber);
    delay(1000);
  }
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
