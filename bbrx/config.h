#pragma once

//  General System Stuff
#define VERSION_STRING          "v1.0"

// ESC PWM settings
#define ESC_PWM_MIN             1000    // minimum pulse width in µs
#define ESC_PWM_MID             1500    // half-way pulse width in µs
#define ESC_PWM_MAX             2000    // maximum pulse width in µs
#define ESC_PWM_FREQ            50      // pwm frequency in Hz

//  Bot Control Settings
#define DEADZONE_LY             32      // inner deadzone for the left analog stick y axis
#define BEEFZONE_LY             500     // outer deadzone for the left analog stick y axis
#define DEADZONE_LX             32      // inner deadzone for the left analog stick x axis
#define BEEFZONE_LX             500     // outer deadzone for the left analog stick x axis

// Pin Definitions
#define PIN_OUT_CH1             12      // pin to use for PWM channel 1 output
#define PIN_OUT_CH2             13      // pin to use for PWM channel 2 output