#ifndef CONFIG_H
#define CONFIG_H

// Joint 0 - Shoulder Yaw 
#define J0_STEPS_PER_DEG 44
#define J0_SPEED 20
#define J0_MIN_SPEED 5
#define J0_ACCEL_RATE 100
#define J0_MAX_ROT_DEG 180
#define J0_ENABLE_HIGH 1

//Joint 1 - Shoulder Pitch
#define J1_STEPS_PER_DEG 60
#define J1_SPEED 10
#define J1_MIN_SPEED 5
#define J1_ACCEL_RATE 100
#define J1_MAX_ROT_DEG 180
#define J1_ENABLE_HIGH 1

//Joint 2 Elbow Pitch
#define J2_STEPS_PER_DEG 60
#define J2_SPEED 12
#define J2_MIN_SPEED 5
#define J2_ACCEL_RATE 100
#define J2_MAX_ROT_DEG 180
#define J2_ENABLE_HIGH 1

// Joint 3 Elbow Roll
#define J3_STEPS_PER_DEG 44
#define J3_SPEED 10
#define J3_MIN_SPEED 5
#define J3_ACCEL_RATE 100
#define J3_MAX_ROT_DEG 180
#define J3_ENABLE_HIGH 0

//Joint 4 Wrist Pitch
#define J4_STEPS_PER_DEG 44
#define J4_SPEED 10
#define J4_MIN_SPEED 5
#define J4_ACCEL_RATE 100
#define J4_MAX_ROT_DEG 180
#define J4_ENABLE_HIGH 1

//Joint 5 Wrist Roll
#define J5_STEPS_PER_DEG 44
#define J5_SPEED 10
#define J5_MIN_SPEED 5
#define J5_ACCEL_RATE 100
#define J5_MAX_ROT_DEG 180
#define J5_ENABLE_HIGH 1

//other constants
#define TOTAL_JOINTS 6
#define BAUD_RATE 115200
#define INST_ARRAY_LEN 20 //length of instruction array
#define INTERRUPT_TIME 100

#endif
