#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

class StepperMotor{
  public:
    StepperMotor();
    StepperMotor(int stepPin, volatile uint8_t *stepPort, uint8_t stepByte, int dirPin, int enablePin, int speed, int minSpeed, int accelRate, bool enableHIGH, bool motorInvert);
    void move(int stepsToMove);
    bool step(unsigned int elapsedMicros, bool contMove);
 
    //setters
    void setSpeed(int speed);
    void setMinSpeed(int minSpeed);
    void setAccelRate(int rate);
    void begin();

    //getters
    int getSpeed(){ return speed; };
    int getSteps(){ return steps; };

  private:
    int stepPin, dirPin, enablePin;
    volatile int steps, currentSpeed, stepsTarget, currentStepDelayDuration, maxStepDelayDuration, speed, minSpeed, accelRate, accelLength;
    unsigned int stepRunTime;
    bool stepDelay, enableHIGH, motorInvert;
    volatile uint8_t *stepPort;
    uint8_t stepByte;
    void updateAccel();
    void updateAccelParams();
};

StepperMotor::StepperMotor(int stepPin, volatile uint8_t *stepPort, uint8_t stepByte, int dirPin, int enablePin, int speed,
      int minSpeed, int accelRate, bool enableHIGH, bool motorInvert){
  this->stepPin = stepPin;
  this->dirPin = dirPin;
  this->enablePin = enablePin;
  this->accelRate = accelRate;
  this->enableHIGH = enableHIGH;
  this->motorInvert = motorInvert;
  this->stepPort = stepPort;
  this->stepByte = stepByte;
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

bool StepperMotor::step(unsigned int elapsedMicros, bool contMove){
  bool stepped = false;
  stepRunTime += elapsedMicros;
  if (steps > 0){
    if(!stepDelay){
      *stepPort = *stepPort & (~stepByte);
      delayMicroseconds(3);
      *stepPort = *stepPort | stepByte;
      stepped = true;
      stepDelay = true;
      if(!contMove){
        steps--;
        }
    }
    else{
      if(stepRunTime > currentStepDelayDuration){
        stepRunTime = 0;
        updateAccel();  
        stepDelay = false;
      }
   }
  }
  return stepped;
}

void StepperMotor::updateAccel(){

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
  //Deceleration for Mid Move Speed Changes
  else if(currentSpeed > speed){
    currentSpeed -= accelRate;
  }

  currentStepDelayDuration = (long)1000000 / (long)currentSpeed;    
}

void StepperMotor::move(int stepsToMove){
  steps = stepsTarget = abs(stepsToMove);
  
  //Set Direction of Motor
  digitalWrite(dirPin, (stepsToMove > 0) ^ motorInvert);
  //Set speed to min
  currentSpeed = minSpeed;
}

void StepperMotor::updateAccelParams(){
  accelLength = (speed-minSpeed)/accelRate;
  stepsTarget = steps;
}

//setters
void StepperMotor::setSpeed(int speed){
  //speed in steps per second
  this->speed = speed;
  updateAccelParams();
}

void StepperMotor::setMinSpeed(int minSpeed){
  //speed in steps per second
  this->minSpeed = minSpeed;
  updateAccelParams();
}

void StepperMotor::setAccelRate(int rate){
  accelRate = rate;
  updateAccelParams();
}

#endif
