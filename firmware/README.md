# AR2 Firmware

## Implemented Features

* Simultaneous movemnt
* Latching Emergency Stop
* 10Hz Status Messages with position updates
* Position Tracking
* Calibration Routine
* Acceleration/Deceleration Profiles

## Commands

* `mNUMBERDEGREES`, moves motor `NUMBER` a set number of `DEGREES`
* `r` resets the emergency stop after being pressed
* `c` starts calibration routine
* `q` quit, ceases all activity
* `tMESSAGE` tool communication, passes `MESSAGE` to the end effector 