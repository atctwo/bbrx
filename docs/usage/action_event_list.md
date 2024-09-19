# Complete List of Actions and Events
This document contains a big list of every implemented action and event.  Detailed descriptions are also given for each action, explaining what they do, what output they may produce, and how to use them.  Each of these are defined in enums in [event_manager.h](../bbrx/event_manager.h).

# Actions (`bb_action`)

| Action                    | Input Type | Description                                                         |
|---------------------------|------------| --------------------------------------------------------------------|
| `BB_ACTION_TEST`          | Digital    | Prints a test message over serial                                   |
| `BB_ACTION_SERVO`         | Analog     | Controls the PWM output of a servo channel over the specified pin   |
| `BB_ACTION_SPEED_UP`      | Digital    | Decreases the speed limit                                           |
| `BB_ACTION_SPEED_DOWN`    | Digital    | Increases the speed limit                                           |
| `BB_ACTION_SPEED_SET`     | Analog     | Sets the motor speed directly                                       |
| `BB_ACTION_BRAKE`         | Digital    | Enables or disables the breaks                                      |
| `BB_ACTION_GPIO`          | Digital    | Writes a digital output to a GPIO pin

Actions are split into two types based on input type:
- **Analog** actions expect a continuous range (in the mathematical sense) of values between `min` and `max`
- **Digital** actions will only interpet the input as `false` or `true` (usually true if the input is greater than the halfway point between `min` and `max`)

## Test Message (`BB_ACTION_TEST`)
Prints a friendly test message.  This is used for development purposes, and optionally can be used for fun?  The message printed will be:
```plain
event manager test!!! pin=<pin number of binding>
```

## Servo PWM (`BB_ACTION_SERVO`)
Most ESCs used in hobbyist RC projects expect motor control signals to be pulse width modulated (PWM).  Specifically, they expect the same type of signal that you would use to control the position of a servo motor, where the position is relative to the pulse width.  Servos typically expect a PWM signal of 50 Hz, where a pulse lasting 1000µs puts the servo at -90°, a pulse lasting 2000µs puts the servo at +90°, and a pulse at 1500µs puts the motor at 0°.  ESCs don't control the position of a motor, instead they control how fast the motor is going; 1000µs means reverse at full speed, 2000µs means forward at full speed, and 1500µs means stop.

Servo channels in bbrx will output this type of signal.  Since every ESC is different, there are a number of parameters in [config.h](../bbrx/config.h) which can be tweaked to work best with your ESC:
- `ESC_PWM_MIN`: minimum pulse width in µs (default is 1000)
- `ESC_PWM_MID`: half-way pulse width in µs (default is 1500)
- `ESC_PWM_MAX`: maximum pulse width in µs (default is 2000)
- `ESC_PWM_FREQ`: pwm frequency in Hz (default is 50)

The PWM data will be output on the pin specified in the `pin` parameter.  Please note that there is a recommended set of pins to use; as per [ESP32Servo](https://github.com/madhephaestus/ESP32Servo)'s documentation:
> All pin numbers are allowed, but only pins 2,4,12-19,21-23,25-27,32-33 are recommended.

## Speed Up (`BB_ACTION_SPEED_UP`) and Speed Down (`BB_ACTION_SPEED_DOWN`)
The speed of the servo output can be limited using the speed actions.  Internally there is a speed limit variable, by which is the number of microseconds the PWM pulse is reduced.  The minimum value is zero (full speed) and the maximum value is `(ESC_PWM_MAX-ESC_PWM_MIN)/2`, which effectively forces the output to be the middle pulse length, preventing the motors from moving at all.  By having the speed limit somewhere between these values, you can control the maximum speed of the motors independently from the motor control inputs.

The Speed Up action reduces the speed limit variable by 1µs, increasing the motors' maximum speed.  Conversely, the Speed Down action increases the speed limit variable by 1µs, reducing the motors' max speed.

These actions are digital, so they consider any input over the halfway point between `min` and `max` to be `true`.  The increase or decrease will be taken for each main loop iteration where the input resolves as true.

## Speed Set (`BB_ACTION_SPEED_SET`)
This is an alternative way of setting the speed limit.  This action takes a continuous analog input, scales it, then writes the output to the speed limit variable.  This allows you to link the motor speed directly to an input.  For an example of how to use this, see the [analog speed control binding example](./events.md#analog-speed-control).

## Brake (`BB_ACTION_BRAKE`)
If the input to this action resolves as true, then all motors will be stopped immediately.  This overrides everything else including speed control.

## GPIO Output (`BB_ACTION_GPIO`)
This action reads the event value as a digital input, then simply writes that value as a digital output to the specified GPIO pin.  This can be used for general purpose applications like powering an LED when a button is pressed.

# Events (`bb_event`)
Gamepad events are based on the inputs exposed by Bluepad32, which bbrx depends on for gamepad support.  Input naming is also carried over directly from BP32.

Note: many of these have not been tested with bbrx.  If you spot anything incorrect here, please raise an issue on GitHub!

## Controller Analog Inputs
| Event                           | Description                                       | Range Minimum | Range Maximum |
|---------------------------------|---------------------------------------------------|---------------|---------------|
| `BB_EVENT_ANALOG_LX`            | Left Analog Stick, X axis                         | -512          | 511           |
| `BB_EVENT_ANALOG_LY`            | Left Analog Stick, Y axis                         | -512          | 511           |
| `BB_EVENT_ANALOG_RX`            | Right Analog Stick, X axis                        | -512          | 511           |
| `BB_EVENT_ANALOG_RY`            | Right Analog Stick, Y axis                        | -512          | 511           |
| `BB_EVENT_ANALOG_THROTTLE`      | Left Analog Trigger (same button as L2)           | 0             | 1023          |
| `BB_EVENT_ANALOG_BRAKE`         | Right Analog Trigger (same button as R2)          | 0             | 1023          |

## Controller Motion Controls
| Event                           | Description                                       | Measurement Unit |
|---------------------------------|---------------------------------------------------|------------------|
| `BB_EVENT_GYRO_X`               | Gyroscope, X axis                                 | degrees / second |
| `BB_EVENT_GYRO_Y`               | Gyroscope, Y axis                                 | degrees / second |
| `BB_EVENT_GYRO_Z`               | Gyroscope, Z axis                                 | degrees / second |
| `BB_EVENT_ACCEL_X`              | Accelerometer, X axis                             | G                |
| `BB_EVENT_ACCEL_Y`              | Accelerometer, Y axis                             | G                |
| `BB_EVENT_ACCEL_Z`              | Accelerometer, Z axis                             | G                |

## D-Pad Buttons
| Event                           | Description                                       | Range Minimum | Range Maximum |
|---------------------------------|---------------------------------------------------|---------------|---------------|
| `BB_EVENT_DPAD_UP`              | Up button on the D-Pad                            | 0             | 1             |
| `BB_EVENT_DPAD_DOWN`            | Down button on the D-Pad                          | 0             | 1             |
| `BB_EVENT_DPAD_LEFT`            | Left button on the D-Pad                          | 0             | 1             |
| `BB_EVENT_DPAD_RIGHT`           | Right button on the D-Pad                         | 0             | 1             |

## Controller Buttons
| Event                           | Xbox          | PlayStation      | Nintendo          | Range Minimum | Range Maximum |
|---------------------------------|---------------|------------------|-------------------|---------------|---------------|
| `BB_EVENT_BTN_A`                | Ⓐ             | ✕               | Ⓑ                 | 0             | 1             |
| `BB_EVENT_BTN_B`                | Ⓑ             | ○               | Ⓐ                 | 0             | 1             |
| `BB_EVENT_BTN_X`                | Ⓧ             | □               | Ⓨ                 | 0             | 1             |    
| `BB_EVENT_BTN_Y`                | Ⓨ             | △              | Ⓧ                 | 0             | 1             |    
| `BB_EVENT_BTN_L1`               | <kbd>LB</kbd> | <kbd>L1</kbd>    | <kbd>L</kbd>      | 0             | 1             |
| `BB_EVENT_BTN_L2`               | <kbd>LT</kbd> | <kbd>L2</kbd>    | <kbd>ZL</kbd>     | 0             | 1             |
| `BB_EVENT_BTN_R1`               | <kbd>RB</kbd> | <kbd>R1</kbd>    | <kbd>R</kbd>      | 0             | 1             |
| `BB_EVENT_BTN_R2`               | <kbd>RT</kbd> | <kbd>R2</kbd>    | <kbd>ZR</kbd>     | 0             | 1             |
| `BB_EVENT_BTN_L3`               | <kbd>LS</kbd> | <kbd>L3</kbd>    | Left Stick Press  | 0             | 1             |
| `BB_EVENT_BTN_R3`               | <kbd>LS</kbd> | <kbd>L3</kbd>    | Right Stick Press | 0             | 1             |
| `BB_EVENT_BTN_SYSTEM`           | Xbox button   | PS button        | HOME button       | 0             | 1             |
| `BB_EVENT_BTN_START`            | Menu button   | Options button   | <kbd>+</kbd>      | 0             | 1             |
| `BB_EVENT_BTN_SELECT`           | View button   | Create button    | <kbd>-</kdb>      | 0             | 1             |
| `BB_EVENT_BTN_CAPTURE`          | Share button  | Touch pad button | Capture button    | 0             | 1             |

## Mouse Events
| Event                           | Description                                       |
|---------------------------------|---------------------------------------------------|
| `BB_EVENT_MOUSE_DX`             | Change in position in the x axis                  |
| `BB_EVENT_MOUSE_DY`             | Change in position in the y axis                  |
| `BB_EVENT_MOUSE_SCROLLWHEEL`    | Change in the position of the scrollwheel         |

## Wii Balance Board Events
| Event                           | Description                                       |
|---------------------------------|---------------------------------------------------|
| `BB_EVENT_WII_BB_TOP_LEFT`      | Top left pressure sensor                          |
| `BB_EVENT_WII_BB_TOP_RIGHT`     | Top right pressure sensor                         |
| `BB_EVENT_WII_BB_BOTTOM_LEFT`   | Bottom left pressure sensor                       |
| `BB_EVENT_WII_BB_BOTTOM_RIGHT`  | Bottom right pressure sensor                      |
| `BB_EVENT_WII_BB_TEMPERATURE`   | Temperature sensor                                |

## Misc
| Event                           | Description                                       | Range Minimum | Range Maximum                                  |
|---------------------------------|---------------------------------------------------|---------------|------------------------------------------------|
| `BB_EVENT_MISC_BATTERY`         | The battery's current charge level                | 0 (empty)     | 254 (full) or 255 (battery info not available) |
