#ifndef JOINT_H
#define JOINT_H

#include "StepperMotor.h"

class Joint{
  public:
  Joint(StepperMotor* stepperMotor_, int switchPin_, int maxRotation_);
  move(int degrees);
  calibrate();

  //setters
  setDelay(int delay);
  setStartingDelay(int delay);
  setProfileSteps(int steps);
  
  bool isCalibrated = false;
  int position = 0; 

  private:
  StepperMotor* stepperMotor;
  int switchPin, maxRotation;
};

Joint::Joint(StepperMotor* stepperMotor_, int switchPin_, int maxRotation_){
	stepperMotor = stepperMotor_;
	switchPin = switchPin_;
  maxRotation = maxRotation_;
	pinMode(switchPin,INPUT);
};

Joint::move(int degrees){
  if(isCalibrated && position + degrees >= 0 && position + degrees <= maxRotation){
    position += degrees;
    stepperMotor->moveMotorDegrees(degrees);
  }else if(!isCalibrated){
    stepperMotor->moveMotorDegrees(degrees);
  }
}

Joint::calibrate(){
  while(!digitalRead(switchPin)){
    stepperMotor->moveMotorDegrees(1);
  }
  isCalibrated = true;
}

//setters
Joint::setDelay(int delay){
  stepperMotor->setDelay(delay);
}

Joint::setStartingDelay(int delay){
  stepperMotor->setStartingDelay(delay);
}

Joint::setProfileSteps(int steps){
  stepperMotor->setProfileSteps(steps);
}

#endif
