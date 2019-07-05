#ifndef PINS_H
#define PINS_H
#include "Arduino.h"

// Joint 0 - Shoulder Yaw 
#define J0_STEP_PORT PORTA
#define J0_STEP_BYTE 0x01
#define J0_STEP 22
#define J0_DIR 23
#define J0_ENABLE 24
#define J0_L_SWITCH 38
#define J0_SWITCH_PORT PIND
#define J0_SWITCH_BYTE 0x80

//Joint 1 - Shoulder Pitch
#define J1_STEP_PORT PORTA
#define J1_STEP_BYTE 0x08
#define J1_STEP 25
#define J1_DIR 26
#define J1_ENABLE 27
#define J1_L_SWITCH 40
#define J1_SWITCH_PORT PING
#define J1_SWITCH_BYTE 0x02

//Joint 2 Elbow Pitch
#define J2_STEP_PORT PORTA
#define J2_STEP_BYTE 0x40
#define J2_STEP 28
#define J2_DIR 29
#define J2_ENABLE 30
#define J2_L_SWITCH 42
#define J2_SWITCH_PORT PINL
#define J2_SWITCH_BYTE 0x80

// Joint 3 Elbow Roll
#define J3_STEP_PORT PORTE
#define J3_STEP_BYTE 0x08
#define J3_STEP 5
#define J3_DIR 6
#define J3_ENABLE 7 
#define J3_L_SWITCH 44
#define J3_SWITCH_PORT PINL
#define J3_SWITCH_BYTE 0x20

//Joint 4 Wrist Pitch
#define J4_STEP_PORT PORTC
#define J4_STEP_BYTE 0x40
#define J4_STEP 31
#define J4_DIR 32
#define J4_ENABLE 33
#define J4_L_SWITCH 46
#define J4_SWITCH_PORT PINL
#define J4_SWITCH_BYTE 0x08

//Joint 5 Wrist Roll
#define J5_STEP_PORT PORTC
#define J5_STEP_BYTE 0x08
#define J5_STEP 34
#define J5_DIR 35
#define J5_ENABLE 36
#define J5_L_SWITCH 48
#define J5_SWITCH_PORT PINL
#define J5_SWITCH_BYTE 0x02

//End Effector Connection
#define END_EFFECTOR_1 8
#define END_EFFECTOR_2 9
#define ESTOP 2

#endif
