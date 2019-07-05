#ifndef CONFIG_H
#define CONFIG_H

// Joint 0 - Shoulder Yaw 
#define J0_JOINT_NUMBER 0
#define J0_STEPS_PER_DEG 44
#define J0_SPEED 80
#define J0_MIN_SPEED 40
#define J0_ACCEL_RATE 8
#define J0_MAX_ROT_DEG 350
#define J0_ENABLE_HIGH 0
#define J0_MOTOR_INVERT 0
#define J0_DEFAULT_POS 175

//Joint 1 - Shoulder Pitch
#define J1_JOINT_NUMBER 1
#define J1_STEPS_PER_DEG 55
#define J1_SPEED 40
#define J1_MIN_SPEED 20
#define J1_ACCEL_RATE 3
#define J1_MAX_ROT_DEG 155
#define J1_ENABLE_HIGH 0
#define J1_MOTOR_INVERT 0
#define J1_DEFAULT_POS 44

//Joint 2 Elbow Pitch
#define J2_JOINT_NUMBER 2
#define J2_STEPS_PER_DEG 55
#define J2_SPEED 50
#define J2_MIN_SPEED 30
#define J2_ACCEL_RATE 8
#define J2_MAX_ROT_DEG 255
#define J2_ENABLE_HIGH 0
#define J2_MOTOR_INVERT 1
#define J2_DEFAULT_POS 150

// Joint 3 Elbow Roll
#define J3_JOINT_NUMBER 3
#define J3_STEPS_PER_DEG 46
#define J3_SPEED 100
#define J3_MIN_SPEED 60
#define J3_ACCEL_RATE 8
#define J3_MAX_ROT_DEG 325
#define J3_ENABLE_HIGH 1
#define J3_MOTOR_INVERT 0
#define J3_DEFAULT_POS 162

//Joint 4 Wrist Pitch
#define J4_JOINT_NUMBER 4
#define J4_STEPS_PER_DEG 22
#define J4_SPEED 100
#define J4_MIN_SPEED 70
#define J4_ACCEL_RATE 8
#define J4_MAX_ROT_DEG 210
#define J4_ENABLE_HIGH 0
#define J4_MOTOR_INVERT 0
#define J4_DEFAULT_POS 80

//Joint 5 Wrist Roll
#define J5_JOINT_NUMBER 5
#define J5_STEPS_PER_DEG 21
#define J5_SPEED 300
#define J5_MIN_SPEED 100
#define J5_ACCEL_RATE 20
#define J5_MAX_ROT_DEG 355
#define J5_ENABLE_HIGH 0
#define J5_MOTOR_INVERT 1
#define J5_DEFAULT_POS 178

//other constants
#define STATUS_FREQ 10
#define TOTAL_JOINTS 6
#define SWITCH_DEBOUNCE_LEN 5 // length of the debounce buffer
#define BAUD_RATE 115200
#define INST_ARRAY_LEN 20 //length of instruction array
#define INTERRUPT_TIME 250 //Polling interupt time (microseconds)

#endif
