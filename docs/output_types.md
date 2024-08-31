# Receiver Action Outputs
This document describes the types of electronic output that each action produces.  Note that not all actions produce any output; only those that make use of the `pin` parameter create any output.  At the minute there is only one type of output that you can produce, but I wanted somewhere to put this paragraph I wrote, and also I might add additional output types in the future :P

# Servo PWM
Most ESCs used in hobbyist RC projects expect motor control signals to be pulse width modulated (PWM).  Specifically, they expect the same type of signal that you would use to control the position of a servo motor, where the position is relative to the pulse width.  Servos typically expect a PWM signal of 50 Hz, where a pulse lasting 1000µs puts the servo at -90°, a pulse lasting 2000µs puts the servo at +90°, and a pulse at 1500µs puts the motor at 0°.  ESCs don't control the position of a motor, instead they control how fast the motor is going; 1000µs means reverse at full speed, 2000µs means forward at full speed, and 1500µs means stop.

Servo channels in bbrx will output this type of signal.  Since every ESC is different, there are a number of parameters in [config.h](../bbrx/config.h) which can be tweaked to work best with your ESC:
- `ESC_PWM_MIN`: minimum pulse width in µs (default is 1000)
- `ESC_PWM_MID`: half-way pulse width in µs (default is 1500)
- `ESC_PWM_MAX`: maximum pulse width in µs (default is 2000)
- `ESC_PWM_FREQ`: pwm frequency in Hz (default is 50)
