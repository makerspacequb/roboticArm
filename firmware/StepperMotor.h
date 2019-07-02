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
    int getSteps(){ return steps; };

  private:
    int stepPin, dirPin, enablePin, speed, minSpeed, accelRate;
    volatile int steps, currentSpeed, stepsTarget, currentStepDelayDuration, maxStepDelayDuration;
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
  stepRunTime = 0;
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
  bool stepped = false;
  stepRunTime += elapsedMicros;
  if (steps > 0){
    if(!stepDelay){
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(5);
      digitalWrite(stepPin, LOW);
      if(!contMove){
        stepDelay = true;
        steps--;
        }
      stepped = true;
    }
    else{
      if((stepRunTime-18) > currentStepDelayDuration){
        stepRunTime = 0;
        updateAcceleration();  
        stepDelay = false;
      }
      stepped = false;
      }
  }
  else {
    stepped = false;
    }
  return stepped;
}

void StepperMotor::updateAcceleration(){

  int accelLength = (speed-minSpeed)/accelRate;
  //If not going to reach top speed
  if((stepsTarget-accelLength)<0){
    accelLength = (stepsTarget/2);
  }
  
  //Acceleration Region
  if(steps > (stepsTarget-accelLength)){
    currentSpeed += accelRate;
  }
  //Deceleration Region
  else if(steps < accelLength){
    currentSpeed -= accelRate;
  }

  currentStepDelayDuration = (long)1000000 / (long) currentSpeed;    
}

void StepperMotor::move(int stepsToMove){
  steps = stepsTarget = abs(stepsToMove);
  
  //Set Direction of Motor
  digitalWrite(dirPin, (stepsToMove > 0) ^ motorInvert);
  //Set speed to min
  currentSpeed = minSpeed;
}

//setters
void StepperMotor::setSpeed(int speed){
  //speed in steps per second
  this->speed = speed;
}

void StepperMotor::setMinSpeed(int minSpeed){
  //speed in steps per second
  this->minSpeed = minSpeed;
  maxStepDelayDuration = (long)1000000 / (long) speed;
}

void StepperMotor::setAccelRate(int rate){
  accelRate = rate;
}

#endif
