#ifndef JOINT_H
#define JOINT_H

#include "Config.h"
#include "StepperMotor.h"

class Joint{
  public:
    Joint(int stepPin, int dirPin, int enablePin, int stepsPerDegree, int speed, int minSpeed, 
          int accelRate, bool enableHIGH, volatile uint8_t *switchPort, uint8_t switchByte, int maxRotation, bool motorInvert);
    void move(float degrees);
    void moveTo(float targetPosition);
    bool calibrate();
    void update(unsigned long elapsedMicros);
    bool checkLimitSwitch(){ return limitSwitchActivated; };
    void resetLimitSwitch(){ limitSwitchActivated = false; };
    float getPosDegrees(){ return positionSteps/stepsPerDegree; };
    void begin();
    
    //setters
    void setSpeed(int speed);
    void setMinSpeed(int speed);
    void setAccelRate(int rate);
    
  private:
    volatile int bufferPos = 0;
    volatile bool switchState = false;
    volatile bool switchBuffer = true;
    volatile bool calibrating = false;
    volatile int positionSteps = 0; 
    int switchPin, maxRotation, stepsPerDegree, minSpeed, speed;
    StepperMotor* stepperMotor;
    bool isCalibrated = false;
    volatile bool limitSwitchActivated, contMoveFlag;
    volatile int movDir;
    volatile uint8_t *switchPort;
    uint8_t switchByte;
};

Joint::Joint(int stepPin, int dirPin, int enablePin, int stepsPerDegree, int speed, int minSpeed, 
             int accelRate, bool enableHIGH, volatile uint8_t *switchPort, uint8_t switchByte, int maxRotation, bool motorInvert){
  int speedStepsPerSec = speed * stepsPerDegree;
  int minSpeedStepsPerSec = minSpeed * stepsPerDegree;
	stepperMotor = new StepperMotor(stepPin, dirPin, enablePin, speedStepsPerSec, minSpeedStepsPerSec, accelRate, enableHIGH, motorInvert);
	this->switchPin = switchPin;
  this->maxRotation = maxRotation;
  this->stepsPerDegree = stepsPerDegree;
  this->minSpeed = minSpeed;
  this->speed = speed;
  contMoveFlag = false;
  limitSwitchActivated = false;
  movDir = 1;
  this->switchPort = switchPort;
  this->switchByte = switchByte;
}

//Needs to be called in setup to initialise pins
void Joint::begin(){
  pinMode(switchPin,INPUT_PULLUP);
  stepperMotor->begin();
}

void Joint::update(unsigned long elapsedMicros){

  //Step Motors and track steps
  if (stepperMotor->step(elapsedMicros, contMoveFlag))
    positionSteps += movDir;
  
  //Poll switch
  // TODO change to port manipulation for better performance
  switchBuffer &= !(*switchPort & switchByte);
  bufferPos++;

  // switch state updates once every SWITCH_DEBOUNCE_LEN
  if(bufferPos >= SWITCH_DEBOUNCE_LEN) {
    switchState = switchBuffer;
    switchBuffer = true;
    bufferPos = 0;
    
    if(switchState && !limitSwitchActivated && !calibrating) {
      //First Time Switch Detected Activated
      //Stop movement 
      stepperMotor->move(0);
      //Reset Position
      positionSteps = 0;
      limitSwitchActivated = true;
    }
    else
      limitSwitchActivated = switchState;
  }
}

void Joint::move(float degrees){
 
  movDir = degrees / abs(degrees);

  //Prevent movement further than the limit switch
  if(limitSwitchActivated && (movDir == -1))
    degrees = 0;

  int steps = degrees * stepsPerDegree;

  // TODO consider effect of calibration
  if(((positionSteps + steps) >= 0) && ((positionSteps + steps) <= (maxRotation*stepsPerDegree)))
    stepperMotor->move(steps);
  else {
    Serial.println("ERROR: Movement out of range.");
  }
}

void Joint::moveTo(float targetPosition){
  int steps = (targetPosition * stepsPerDegree) - positionSteps;
  if((targetPosition <= maxRotation) && (targetPosition >= 0))
    move(steps/stepsPerDegree);
}

bool Joint::calibrate(){
  
  setSpeed(minSpeed);
  int maxSteps = maxRotation * stepsPerDegree;
    
  movDir = -1;
  positionSteps = maxSteps;
  
  if(!limitSwitchActivated) 
    stepperMotor->move(-maxSteps);
  
  calibrating = true;
  while(!limitSwitchActivated && (positionSteps > 0)){
  }
  
  stepperMotor->move(0);
  positionSteps = 0;
  setSpeed(speed);
  
  if(limitSwitchActivated) {
    isCalibrated = true;
    stepperMotor->move(maxSteps/2);
  }
  else{
    isCalibrated = false;
  }

  calibrating = false;
  return isCalibrated;
}

//setters
void Joint::setSpeed(int speed){
  this->speed = speed;
  stepperMotor->setSpeed(speed * stepsPerDegree);
}

void Joint::setMinSpeed(int speed){
  this->minSpeed = speed;
  stepperMotor->setMinSpeed(speed * stepsPerDegree);
}

void Joint::setAccelRate(int rate){
  stepperMotor->setAccelRate(rate);
}

#endif
