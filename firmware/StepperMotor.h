#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

class StepperMotor{
  public:
    StepperMotor();
    StepperMotor(int stepPin, int dirPin, int enablePin, int speed, int minSpeed, int accelRate, bool enableHIGH, bool motorInvert);
    void move(int stepsToMove);
    bool step(unsigned long elapsedMicros, bool contMove);
  
    //setters
    void setSpeed(int speed);
    void setMinSpeed(int minSpeed);
    void setAccelRate(int rate);
    void begin();

    //getters
    int getSpeed(){ return speed; };

  private:
    int stepPin, dirPin, enablePin, speed, minSpeed, accelRate;
    volatile int steps, currentStepDelayDuration, maxStepDelayDuration, stepDelayDuration;
    unsigned long stepRunTime;
    bool stepDelay, enableHIGH, motorInvert;
    void updateAcceleration();
};

StepperMotor::StepperMotor(int stepPin, int dirPin, int enablePin, int speed,
      int minSpeed, int accelRate, bool enableHIGH, bool motorInvert){
  this->stepPin = stepPin;
  this->dirPin = dirPin;
  this->enablePin = enablePin;
  this->accelRate = accelRate;
  this->enableHIGH = enableHIGH;
  this->motorInvert = motorInvert;
  setSpeed(speed);
  setMinSpeed(minSpeed);
  steps = 0;
  stepDelay = false;
  currentStepDelayDuration = maxStepDelayDuration;
  
}

//Needs to be called in setup to initialise pins
void StepperMotor::begin(){
  pinMode(enablePin,OUTPUT);
  pinMode(stepPin,OUTPUT);
  pinMode(dirPin,OUTPUT);
  digitalWrite(enablePin,enableHIGH);
}

bool StepperMotor::step(unsigned long elapsedMicros, bool contMove){
  stepRunTime += elapsedMicros;
  if (steps > 0) {
    if(!stepDelay) {
      // TODO replace these with port manipulation
      // digital write will take about 6us
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(5);
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

//TODO - Needs Rework to prevent sawtooth behavior
void StepperMotor::updateAcceleration(){
  //update acceleration
  if(steps * currentStepDelayDuration / accelRate < maxStepDelayDuration - currentStepDelayDuration){
    //deceleration
    if(currentStepDelayDuration < maxStepDelayDuration){
      //currentStepDelayDuration += currentStepDelayDuration / accelRate;
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

void StepperMotor::move(int stepsToMove){
  steps = abs(stepsToMove);
  digitalWrite(dirPin, (stepsToMove > 0) ^ motorInvert);
  currentStepDelayDuration = maxStepDelayDuration;
}

//setters
void StepperMotor::setSpeed(int speed){
  //speed in steps per second
  this->speed = speed;
  stepDelayDuration = (long)1000000 / (long) speed;
}

void StepperMotor::setMinSpeed(int minSpeed){
  //speed in steps per second
  this->minSpeed = minSpeed;
  maxStepDelayDuration = (long)1000000 / (long)minSpeed;
}

void StepperMotor::setAccelRate(int rate){
  accelRate = rate;
}

#endif
