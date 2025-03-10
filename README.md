# Lockit

Goal of this project is to develop a TOTP-Based lock for securing the snacks in the Engineering Physics club room.

## Hardware

Schematic and PCB are designed in KiCAD.

### Requirements

- Must fail safe (i.e. when power is off it must stay locked)
- Must be accessible with a physical key when either on or off
- Must have an RTC with battery backup
    - RTC battery backup must be rechargable such that the battery never needs replacing
    - Battery backup must be able to power RTC for very long time (full month maybe?)
- Must not have internet connection
- Must be tamper proof
- Must have an umbilical cord for RTC time syncing
- Must be compatible with fridge and cabinet
- Must have non-volatile memory storage to store seven years of logs
- Must be able to sense when door is open and when door is closed
    - Closed meaning in a position where the lock, whe initiated, will properly close
- Must have speaker to play sounds and alarms
- Should have a USBC PD for negotiation of 5V and 9V
    - Lock solenoid works off of the 9V
    - Embedded System works of 5V

## Software

### Requirements

- Timesync, getting TOTP setup keys, and resetting setup keys, and viewing log must be simple and should be able ot be done on any laptop or computer without needing a dedicated program that needs to be installed
- Should log power on/off, timestamps of door open and close
- Should play an alarm if door is left open and unlocked

### Build system

The goal of the build system was to completely avoid using STMCubeMX IDE.

[STM32_Base_Project](https://github.com/rgujju/STM32_Base_Project/tree/master) was heavily used as a reference for getting the initial build system set up with CMAKE.

[Building for and debugging an ARM Cortex-M](https://blog.peramid.es/posts/2024-12-31-arm.html) was used as a reference for getting semihosting to work properly.