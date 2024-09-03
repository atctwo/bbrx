# Complete List of Actions and Events
This document contains a big list of every implemented action and event.  Each of these are defined in enums in [event_manager.h](../bbrx/event_manager.h).

Note: many of these have not been tested with bbrx.  If you spot anything incorrect here, please raise an issue on GitHub!

## Actions (`bb_action`)

| Action                    | Input Type | Description                                                         |
|---------------------------|------------| --------------------------------------------------------------------|
| `BB_ACTION_TEST`          | Digital    | Prints a test message over serial                                   |
| `BB_ACTION_SERVO`         | Analog     | Controls the PWM output of a servo channel over the specified pin   |
| `BB_ACTION_SPEED_UP`      | Digital    | Decreases the speed limit                                           |
| `BB_ACTION_SPEED_DOWN`    | Digital    | Increases the speed limit                                           |
| `BB_ACTION_BREAK`         | Digital    | Enables or disables the breaks                                      |

Actions are split into two types based on input type:
- **Analog** actions expect a continuous range (in the mathematical sense) of values between `min` and `max`
- **Digital** actions will only interpet the input as `false` or `true` (usually true if the input is greater than the halfway point between `min` and `max`)

## Events (`bb_event`)
Gamepad events are based on the inputs exposed by Bluepad32, which bbrx depends on for gamepad support.  Input naming is also carried over directly from BP32.

### Controller Analog Inputs
| Event                           | Description                                       | Range Minimum | Range Maximum |
|---------------------------------|---------------------------------------------------|---------------|---------------|
| `BB_EVENT_ANALOG_LX`            | Left Analog Stick, X axis                         | -512          | 511           |
| `BB_EVENT_ANALOG_LY`            | Left Analog Stick, Y axis                         | -512          | 511           |
| `BB_EVENT_ANALOG_RX`            | Right Analog Stick, X axis                        | -512          | 511           |
| `BB_EVENT_ANALOG_RY`            | Right Analog Stick, Y axis                        | -512          | 511           |
| `BB_EVENT_ANALOG_THROTTLE`      | Left Analog Trigger (same button as L2)           | 0             | 1023          |
| `BB_EVENT_ANALOG_BRAKE`         | Right Analog Trigger (same button as R2)          | 0             | 1023          |

### Controller Motion Controls
| Event                           | Description                                       | Measurement Unit |
|---------------------------------|---------------------------------------------------|------------------|
| `BB_EVENT_GYRO_X`               | Gyroscope, X axis                                 | degrees / second |
| `BB_EVENT_GYRO_Y`               | Gyroscope, Y axis                                 | degrees / second |
| `BB_EVENT_GYRO_Z`               | Gyroscope, Z axis                                 | degrees / second |
| `BB_EVENT_ACCEL_X`              | Accelerometer, X axis                             | G                |
| `BB_EVENT_ACCEL_Y`              | Accelerometer, Y axis                             | G                |
| `BB_EVENT_ACCEL_Z`              | Accelerometer, Z axis                             | G                |

### D-Pad Buttons
| Event                           | Description                                       | Range Minimum | Range Maximum |
|---------------------------------|---------------------------------------------------|---------------|---------------|
| `BB_EVENT_DPAD_UP`              | Up button on the D-Pad                            | 0             | 1             |
| `BB_EVENT_DPAD_DOWN`            | Down button on the D-Pad                          | 0             | 1             |
| `BB_EVENT_DPAD_LEFT`            | Left button on the D-Pad                          | 0             | 1             |
| `BB_EVENT_DPAD_RIGHT`           | Right button on the D-Pad                         | 0             | 1             |

### Controller Buttons
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

### Mouse Events
| Event                           | Description                                       |
|---------------------------------|---------------------------------------------------|
| `BB_EVENT_MOUSE_DX`             | Change in position in the x axis                  |
| `BB_EVENT_MOUSE_DY`             | Change in position in the y axis                  |
| `BB_EVENT_MOUSE_SCROLLWHEEL`    | Change in the position of the scrollwheel         |

### Wii Balance Board Events
| Event                           | Description                                       |
|---------------------------------|---------------------------------------------------|
| `BB_EVENT_WII_BB_TOP_LEFT`      | Top left pressure sensor                          |
| `BB_EVENT_WII_BB_TOP_RIGHT`     | Top right pressure sensor                         |
| `BB_EVENT_WII_BB_BOTTOM_LEFT`   | Bottom left pressure sensor                       |
| `BB_EVENT_WII_BB_BOTTOM_RIGHT`  | Bottom right pressure sensor                      |
| `BB_EVENT_WII_BB_TEMPERATURE`   | Temperature sensor                                |

### Misc
| Event                           | Description                                       | Range Minimum | Range Maximum                                  |
|---------------------------------|---------------------------------------------------|---------------|------------------------------------------------|
| `BB_EVENT_MISC_BATTERY`         | The battery's current charge level                | 0 (empty)     | 254 (full) or 255 (battery info not available) |
