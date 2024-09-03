#pragma once

//  General System Stuff
#define VERSION_STRING          "v1.0"

// ESC PWM settings
#define ESC_PWM_MIN             1000    // minimum pulse width in µs
#define ESC_PWM_MID             1500    // half-way pulse width in µs
#define ESC_PWM_MAX             2000    // maximum pulse width in µs
#define ESC_PWM_FREQ            50      // pwm frequency in Hz

// failsafes
// you can globally disable all failsafes by commenting out ENABLE_FAILSAFES,
// or you can disable specific failsafes by uncommenting their specific define
#define ENABLE_FAILSAFES                // when defined, failsafes will be enabled.  PLEASE DON'T DISABLE THIS UNLESS YOU REALLY REALLY REALLY NEED TO!!!!!!!PLEASE
#define FAILSAFE_NO_CONTROLLER          // enables the failsafe where if no controllers are detected, the motors will be killed

//  Bot Control Settings

// each binding specifies a minimum and maximum value for the input range
// deadzone is the value below which the input defaults to 0
// beefzone is the value above which the input defaults to max(range_min, range_max), _and_
//             the value _below_ which the input defaults to min(range_min, range_max)
#define DEADZONE_LY             32      // inner deadzone for the left analog stick y axis
#define BEEFZONE_LY             500     // outer deadzone for the left analog stick y axis
#define DEADZONE_LX             32      // inner deadzone for the left analog stick x axis
#define BEEFZONE_LX             500     // outer deadzone for the left analog stick x axis

#define DEADZONE_BRAKE          64      // inner deadzone for the analog brake (L2)
#define BEEFZONE_BRAKE          1000    // outer deadzone for the analog brake (L2)
#define DEADZONE_THROTTLE       64      // inner deadzone for the analog throttle (R2)
#define BEEFZONE_THROTTLE       1000    // outer deadzone for the analog throttle (R2)

// Pin Definitions
#define PIN_OUT_CH1             12      // pin to use for PWM channel 1 output
#define PIN_OUT_CH2             13      // pin to use for PWM channel 2 output