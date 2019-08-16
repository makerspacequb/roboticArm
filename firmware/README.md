# AR2 Firmware

New object-orientated firmware for the AR2 robot designed to be run on an Arduino Mega 2560 at 115200 BAUD.

## Implemented Features

* Simultaneous movemnt
* Latching Emergency Stop
* 10Hz Status Messages with position updates
* Position Tracking
* Calibration Routine
* Acceleration/Deceleration Profiles

## Commands

* `mNUMBERDEGREES`, moves motor `NUMBER` a set number of `DEGREES`.
* `pNUMBERDEGREES`, moves motor `NUMBER` to a set position within range using `DEGREES`.
* `r` resets the emergency stop after being pressed.
* `ca` starts calibration routine for entire arm.
* `cNUMBER` starts calibration routine for motor `NUMBER`.
* `q` quit, ceases all activity.
* `tMESSAGE` tool communication, passes `MESSAGE` to the end effector.
* `sNUMBERSPEED`, sets the motor `NUMBER` speed as `SPEED` in degrees per second
* `dNUMBERSPEED`, sets the motor `NUMBER` minimum speed `SPEED` in degrees per second. This is the speed the acceleration profile begins at.
* `zNUMBERACCEL`, sets the motor `NUMBER` acceleration `ACCEL` in degrees per second squared.
* `i`, returns status update on the arm. Information includes claibration state of each motor, current positions, movement flags and limit switch states.
* `hPERCENT`, when a servo hand tool is used. The percentage open the pincers are is given by a value `PERCENT`.
