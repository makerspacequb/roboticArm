#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

class StepperMotor{
  public:
    StepperMotor();
    StepperMotor(int stepPin, int dirPin, int enablePin, int stepsPerDegree, int speed, int minSpeed, float accelRate);
    void moveDegrees(int degreesToTurn);
    void move(int stepsToMove);
    bool step(unsigned long elapsedMicros, bool contMove);
  
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
}

bool StepperMotor::step(unsigned long elapsedMicros, bool contMove){
  stepRunTime += elapsedMicros;
  if (steps > 0) {
    if(!stepDelay) {
      digitalWrite(stepPin, HIGH);
      // digital write is slow enough to not need a delay.
      // digital write will take about 6us
      digitalWrite(stepPin, LOW);
      if (!contMove)
        steps--;
      stepDelay = true;
      return true;
    }
    else {
      if(currentStepDelayDuration < stepRunTime){
        stepRunTime = 0;
        updateAcceleration();
        stepDelay = false;
      }
    }
  }
  return false;
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

void StepperMotor::moveDegrees(int degreesToTurn){
  steps = abs(degreesToTurn) * stepsPerDegree;
  digitalWrite(dirPin, degreesToTurn > 0);
  currentStepDelayDuration = maxStepDelayDuration;
}

void StepperMotor::move(int stepsToMove){
  steps = abs(stepsToMove);
  digitalWrite(dirPin, stepsToMove > 0);
  currentStepDelayDuration = maxStepDelayDuration;
}

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
