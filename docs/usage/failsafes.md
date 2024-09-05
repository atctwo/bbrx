# Failsafes
bbrx is responsible for controlling motors and other components which could potentially be harmful if operated incorrectly.  Sometimes, however, something electrical or in software breaks down, like a bug in the code, or a controller running out of battery.  In these cases, the best course of action is to just shut everything down to prevent harm.

As such, bbrx includes a number of failsafes.  These are special behaviours that are taken under certain conditions to ensure that the device doesn't harm anyone or go out of control.  The failsafes are programmed straight into bbrx, bypassing the event binding system.  Having said that, each failsafe can be individually disabled by commenting out the associated `#define` in [config.h](../bbrx/config.h).  Every failsafe can be disabled globally by commenting out `#define ENABLE_FAILSAFES`, but in most cases this is a really bad idea so don't do it.

This document describes each of the implemented failsafes and their behaviours and conditions.

## Kill Motors When No Controllers Are Connected (`FAILSAFE_NO_CONTROLLER`)
When enabled, this failsafe will simply stop every servo motor by continuously sending it the midpoint PWM value, when zero controllers are currently connected.  The motors don't get powered down, they just get set to 0 RPM.

When a binding is made to a servo channel, the `Servo` object is stored in a `std::map` along with it's pin number.  This failsafe simply iterates over the `servos` map, calling `writeMicroseconds(ESC_PWM_MID)` for each object.  This means that each servo that is bound to any input will be affected.