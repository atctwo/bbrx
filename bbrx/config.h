#pragma once

//  General System Stuff
#define VERSION_STRING          "v1.0"

// ESC PWM settings
#define ESC_PWM_MIN             1000    // minimum pulse width in µs
#define ESC_PWM_MAX             2000    // maximum pulse width in µs
#define ESC_PWM_FREQ            50      // pwm frequency in Hz

//  Bot Control Settings
#define DEADZONE_LY             32
#define BEEFZONE_LY             500
#define DEADZONE_LX             32
#define BEEFZONE_LX             500

// Pin Definitions
#define PIN_OUT_CH1             12
#define PIN_OUT_CH2             13