#ifndef JOINT_H
#define JOINT_H

#include "StepperMotor.h"

class Joint{
  public:
  Joint(StepperMotor* stepperMotor_, int switchPin_);
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
  int switchPin;
};

Joint::Joint(StepperMotor* stepperMotor_, int switchPin_){
	stepperMotor = stepperMotor_;
	switchPin = switchPin_;
	pinMode(switchPin,INPUT);
};

Joint::move(int degrees){
  if(isCalibrated){
    position += degrees;
  }
  stepperMotor->moveMotorDegrees(degrees);
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
