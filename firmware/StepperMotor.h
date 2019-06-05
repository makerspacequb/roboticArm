#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

class StepperMotor{
  public:
  StepperMotor(){};
  StepperMotor(int stepPin_, int dirPin_, int enablePin_, bool direction_, int stepsPerDegree_, int delayBetweenStep_, int startingDelay_, int profileSteps_);
  moveMotorSteps(int distance);
  moveMotorDegrees(int degreesToTurn);

  //setters
  setDelay(int delay);
  setStartingDelay(int delay);
  setProfileSteps(int steps);

  private:
  int stepPin, dirPin, enablePin, delayBetweenStep, stepsPerDegree;
  //delay within steps (setting pin HIGH->LOW)
  const int delay = 10;
  bool inverted;
  //acc/dec profile
  int startingDelay, profileSteps, currentDelay, profileGradient;
  int getCurrentDelay(int remainingSteps);
};

StepperMotor::StepperMotor(int stepPin_, int dirPin_, int enablePin_, bool inverted_, int stepsPerDegree_, int delayBetweenStep_, int startingDelay_, int profileSteps_){
  stepPin = stepPin_;
  dirPin = dirPin_;
  enablePin = enablePin_;
  inverted = inverted_;
  delayBetweenStep = delayBetweenStep_;
  stepsPerDegree = stepsPerDegree_;
  startingDelay = startingDelay_;
  profileSteps = profileSteps_;
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

//setters
StepperMotor::setDelay(int delay){
	delayBetweenStep = delay;
}

StepperMotor::setStartingDelay(int delay){
  startingDelay = delay;
}

StepperMotor::setProfileSteps(int steps){
  profileSteps = steps;
}
#endif
