#ifndef JOINT_H
#define JOINT_H

#include "pins.h"
#include "Config.h"
#include "StepperMotor.h"
#include "Arduino.h"

class Joint{
  public:
    Joint(int stepPin, volatile uint8_t *stepPort, uint8_t stepByte, int dirPin, int enablePin, int stepsPerDegree, int speed, int minSpeed, 
          int accelRate, bool enableHIGH, int switchPin, volatile uint8_t *switchPort, uint8_t switchByte, int maxRotation, bool motorInvert, int defaultPos);
    void move(float degrees);
    void moveTo(float targetPosition);
    bool calibrate();
    void update(unsigned int elapsedMicros);
    bool checkLimitSwitch(){ return limitSwitchActivated; };
    bool checkMovement() { return movementFlag; };
    bool checkCalibration(){ return isCalibrated; };
    void resetCalibration(){ isCalibrated = false; };
    void resetLimitSwitch(){ limitSwitchActivated = false; };
    float getPosDegrees(){ return positionSteps/stepsPerDegree; };
    volatile int positionSteps = 0; 
    void begin();
    
    //setters
    void setSpeed(int speed);
    void setMinSpeed(int speed);
    void setAccelRate(int rate);
    
  private:
    volatile int bufferPos = 0;
    volatile bool movementFlag = false;
    volatile bool switchState = false;
    volatile bool switchBuffer = true;
    volatile bool calibrating = false;
    int switchPin, maxRotation, stepsPerDegree, defaultPos, minSpeed, speed;
    StepperMotor* stepperMotor;
    bool isCalibrated = false;
    volatile bool limitSwitchActivated = false, contMoveFlag = false;
    volatile int movDir;
    volatile uint8_t *switchPort;
    uint8_t switchByte;
};

Joint::Joint(int stepPin, volatile uint8_t *stepPort, uint8_t stepByte, int dirPin, int enablePin, int stepsPerDegree, int speed, int minSpeed, 
             int accelRate, bool enableHIGH, int switchPin, volatile uint8_t *switchPort, uint8_t switchByte, int maxRotation, bool motorInvert, int defaultPos){
  int speedStepsPerSec = speed * stepsPerDegree;
  int minSpeedStepsPerSec = minSpeed * stepsPerDegree;
	stepperMotor = new StepperMotor(stepPin, stepPort, stepByte, dirPin, enablePin, speedStepsPerSec, minSpeedStepsPerSec, accelRate, enableHIGH, motorInvert);
	this->switchPin = switchPin;
  this->maxRotation = maxRotation;
  this->stepsPerDegree = stepsPerDegree;
  this->minSpeed = minSpeed;
  this->speed = speed;
  this->defaultPos = defaultPos;
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

void Joint::update(unsigned int elapsedMicros){

  if(stepperMotor->getSteps() > 0){
    movementFlag = true;
  }
  else{
    movementFlag = false;
  }
  
  //Step Motors and track steps
  if (stepperMotor->step(elapsedMicros, contMoveFlag)){
    positionSteps += movDir;
  }

  //Poll switch
  switchBuffer &= !(*switchPort & switchByte);
  bufferPos++;

  // switch state updates once every SWITCH_DEBOUNCE_LEN
  if(bufferPos >= SWITCH_DEBOUNCE_LEN){
    switchState = switchBuffer;
    switchBuffer = true;
    bufferPos = 0;
  
    //First Time Switch Detected Activated
    if(switchState && !limitSwitchActivated && !calibrating){
      //Stop movement 
      if(movDir == -1){
        stepperMotor->move(0);
      }
      limitSwitchActivated = true;
    }
    else{
      limitSwitchActivated = switchState;
    }
  }
}

void Joint::move(float degrees){
 
  movDir = degrees / abs(degrees);

  //Prevent movement further than the limit switch
  if(limitSwitchActivated && (movDir == -1))
    degrees = 0;

  int steps = degrees * stepsPerDegree;

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

  int initialSpeed = speed;
  int initialMinSpeed = minSpeed;
  int speedDelta = 10;
  
  setSpeed(minSpeed);
  setSpeed(minSpeed-speedDelta);
  
  int maxSteps = maxRotation * stepsPerDegree;
    
  movDir = -1;
  positionSteps = maxSteps;
  
  if(!limitSwitchActivated){ 
    stepperMotor->move(-maxSteps);
  }
  
  while(!limitSwitchActivated && (positionSteps > 0)){
  }
  
  stepperMotor->move(0);
  positionSteps = 0;
  setSpeed(initialSpeed);
  setMinSpeed(initialMinSpeed);
  
  if(limitSwitchActivated) {
    calibrating = true;
    isCalibrated = true;
    movDir = 1;
    moveTo(defaultPos);
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
