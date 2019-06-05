#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

class StepperMotor{
  public:
  StepperMotor(){};
  StepperMotor(int stepPin_, int dirPin_, int enablePin_, bool direction_, int delayBetweenStep_, int stepsPerDegree_);
  moveMotorSteps(int distance);
  moveMotorDegrees(int degreesToTurn);
  setDelay(int delay);

  private:
  int stepPin, dirPin, enablePin, delayBetweenStep, stepsPerDegree;
  const int delay = 10;
  bool inverted;
  //acc/dec profile
  const int startingDelay = 10000;
  const int profileSteps = 200;
  int currentDelay;
  int profileGradient;
  int getCurrentDelay(int remainingSteps);
};

StepperMotor::StepperMotor(int stepPin_, int dirPin_, int enablePin_, bool inverted_, int delayBetweenStep_, int stepsPerDegree_){
  stepPin = stepPin_;
  dirPin = dirPin_;
  enablePin = enablePin_;
  inverted = inverted_;
  delayBetweenStep = delayBetweenStep_;
  stepsPerDegree = stepsPerDegree_;
  pinMode(stepPin,OUTPUT);
  pinMode(dirPin,OUTPUT);
};

int StepperMotor::getCurrentDelay(int remainingSteps){
  if(remainingSteps <= profileSteps){
    //deceleration
    if(currentDelay < startingDelay){
      currentDelay += profileGradient;
    }
  }else if(currentDelay > delayBetweenStep){
    //acceleration
    currentDelay -= profileGradient;
  }
  return currentDelay;
}

StepperMotor::moveMotorSteps(int distance){
  currentDelay = startingDelay;
  profileGradient = (startingDelay - delayBetweenStep) / profileSteps;
  if(distance < 0){
    digitalWrite(dirPin,inverted);
  } else {
    digitalWrite(dirPin,!inverted);
  }
  for(int i = 0; i < abs(distance); i++){
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(delay);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(getCurrentDelay(abs(distance)-i));
  }
};

StepperMotor::moveMotorDegrees(int degreesToTurn){
  moveMotorSteps(degreesToTurn * stepsPerDegree);
};

StepperMotor::setDelay(int delay){
	delayBetweenStep = delay;
}
#endif
