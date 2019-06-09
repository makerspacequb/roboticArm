#include "pins.h"

//calibration & continuous movement flags
bool isCalibrated = false;
void setup() { 
  pinMode(HAND_PIN,OUTPUT);
  hand.attach(HAND_PIN);
  Serial.begin(BAUD_RATE);
 
  //set timer interrupt for motor and encoder control
  Timer1.attachInterrupt(interrupt);
  Timer1.initialize(INTERRUPT_TIME);
  Timer1.start();
}

void loop() {
  //printSwitches();
  readSerial();
  
  if(continuousMovement){
    for(int i = 0; i < 6; i++){
      moveJoint(i,continuousMovementSpeeds[i]);
    }
  }
}

void readSerial(){
  if (Serial.available() > 0){
    char nextChar = Serial.read();
    //if char is new line last instruction complete, process instruction
    if(nextChar == '\n'){
      if(instIndex > 0) {
        //make sure rest of instruction is cleared
        for (int i = instIndex; i < INST_ARRAY_LEN; i++)
          instruction[i] = NULL;
        //send instruction for processing
        processInstruction(instruction);
        instIndex = 0;
      }
    }
    //add to instruction string
    else {
      if(instIndex >= INST_ARRAY_LEN)
        Serial.println("ERROR - loop() instruction parser: Instruction index out of bounds.");
      else{
        instruction[instIndex] = nextChar;
        instIndex++;
      }
    }
  }
}
