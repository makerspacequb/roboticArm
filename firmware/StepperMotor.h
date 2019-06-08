#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

class StepperMotor{
  public:
    StepperMotor(){};
    StepperMotor(int stepPin, int dirPin, int enablePin, int stepsPerDegree, int speed, int minSpeed, float accelRate);
    void moveMotorSteps(int distance);
    void moveMotorDegrees(int degreesToTurn);
    void step(unsigned long elapsedMicros);
  
    //setters
    void setSpeed(int speed);
    void setMinSpeed(int minSpeed);
    void setAccelRate(int rate);

    //getters
    int getSpeed(){ return speed; };

  private:
    int stepPin, dirPin, enablePin, speed, stepsPerDegree, minSpeed, accelRate;
    volatile int steps, currentStepDelayDuration, maxStepDelayDuration, stepDelayDuration;
    unsigned long stepRunTime;
    bool stepDelay;
    void updateAcceleration();
};

StepperMotor::StepperMotor(int stepPin, int dirPin, int enablePin, int stepsPerDegree, int speed,
      int minSpeed, float accelRate){
  this->stepPin = stepPin;
  this->dirPin = dirPin;
  this->enablePin = enablePin;
  this->stepsPerDegree = stepsPerDegree;
  this->accelRate = accelRate;
  setSpeed(speed);
  setMinSpeed(minSpeed);
  steps = 0;
  stepDelay = false;
  currentStepDelayDuration = maxStepDelayDuration;
  
  pinMode(stepPin,OUTPUT);
  pinMode(dirPin,OUTPUT);
};

void StepperMotor::step(unsigned long elapsedMicros){
  stepRunTime += elapsedMicros;
  if (steps > 0) {
    if(!stepDelay) {
      digitalWrite(stepPin, HIGH);
      // digital write is slow enough to not need a delay.
      // digital write will take about 6us
      digitalWrite(stepPin, LOW);
      steps--;
      stepDelay = true;
    }
    else {
      if(currentStepDelayDuration < stepRunTime){
        stepRunTime = 0;
        updateAcceleration();
        stepDelay = false;
      }
    }
  }
}

void StepperMotor::updateAcceleration(){
  //update acceleration
  if(steps * currentStepDelayDuration / accelRate < maxStepDelayDuration - currentStepDelayDuration){
    //deceleration
    if(currentStepDelayDuration < maxStepDelayDuration){
      currentStepDelayDuration += currentStepDelayDuration / accelRate;
    }
  }else if(currentStepDelayDuration > stepDelayDuration){
    //acceleration  
    currentStepDelayDuration -= (-(currentStepDelayDuration - maxStepDelayDuration) + stepDelayDuration) / accelRate;
  }
  
  //constrain step delay
  if(currentStepDelayDuration < stepDelayDuration) 
    currentStepDelayDuration = stepDelayDuration;
  else if (currentStepDelayDuration > maxStepDelayDuration)
    currentStepDelayDuration = maxStepDelayDuration;
}

void StepperMotor::moveMotorDegrees(int degreesToTurn){
  steps = abs(degreesToTurn) * stepsPerDegree;
  if(degreesToTurn < 0){
    digitalWrite(dirPin,false);
  } else {
    digitalWrite(dirPin,true);
  }
  currentStepDelayDuration = maxStepDelayDuration;
};

//setters
void StepperMotor::setSpeed(int speed){
  //speed in degrees per second
  this->speed = speed;
  stepDelayDuration = (long)1000000 / ((long)stepsPerDegree * (long) speed);
}

void StepperMotor::setMinSpeed(int minSpeed){
  this->minSpeed = minSpeed;
  maxStepDelayDuration = (long)1000000 / ((long)stepsPerDegree * (long)minSpeed);
}

void StepperMotor::setAccelRate(int rate){
  accelRate = rate;
}

#endif
