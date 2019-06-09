# Wheel Base Movement Firmware

Arduino Mega Firmware for current control of the Wheelchair Base

## Usage

1. Arduino Mega Serial line at 115200 BAUD with NL and CR
2. Commands Passed as follows; `SPEED,ANGLE,COMMAND`
3. Speed and Angle use values between -100 and +100 for full range of movement
4. Possible commands include, `RUN`, `SEND`, `STOP`, `RESET`, `BRAKEOFF`

## Commands Usage

The definition of each of the commands are as follows;

* `RUN` - Executes the Speed and Angle without a response.
* `SEND` - Executes Speed and Angle sending back wheelchair info including system voltage, motor currents and status messages.
* `BRAKEOFF` - Turns off wheelchair brake, whilst not executing Speed and Angle.
* `STOP` - Imediately stops the wheelchair and applies brakes - used for emergency stopping - no further commands are accepted unitl a reset has been performed.
* `RESET` - Resets the wheelchair after an emergency stop.