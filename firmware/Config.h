#ifndef CONFIG_H
#define CONFIG_H

// Joint 0 - Shoulder Yaw 
#define J0_STEPS_PER_DEG 44
#define J0_DELAY 500
#define J0_START_DELAY 10000
#define J0_PROFILE_STEPS 300
#define J0_MAX_ROT_DEG 180

//Joint 1 - Shoulder Pitch
#define J1_STEPS_PER_DEG 60
#define J1_DELAY 600
#define J1_START_DELAY 10000
#define J1_PROFILE_STEPS 300
#define J1_MAX_ROT_DEG 180

//Joint 2 Elbow Pitch
#define J2_STEPS_PER_DEG 60
#define J2_DELAY 600
#define J2_START_DELAY 10000
#define J2_PROFILE_STEPS 300
#define J2_MAX_ROT_DEG 180

// Joint 3 Elbow Roll
#define J3_STEPS_PER_DEG 44
#define J3_DELAY 300
#define J3_START_DELAY 10000
#define J3_PROFILE_STEPS 300
#define J3_MAX_ROT_DEG 180

//Joint 4 Wrist Pitch
#define J4_STEPS_PER_DEG 44
#define J4_DELAY 300
#define J4_START_DELAY 10000
#define J4_PROFILE_STEPS 300
#define J4_MAX_ROT_DEG 180

//Joint 5 Wrist Roll
#define J5_STEPS_PER_DEG 44
#define J5_DELAY 300
#define J5_START_DELAY 10000
#define J5_PROFILE_STEPS 300
#define J5_MAX_ROT_DEG 180

//other constants
#define TOTAL_JOINTS 4
#define BAUD_RATE 115200
#define INST_ARRAY_LEN 20 //length of instruction array

#endif
