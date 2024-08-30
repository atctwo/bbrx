# Events and Binding
bbrx allows users to "bind" **receiver actions** to zero or more **gamepad events**.  This means that users can configure bbrx depending on their specific controller and device.  An example set of binds could be:
- bind Ch1 to Left Analog Y
- bind Ch2 to Left Analog X
- bind Ch3 to L2
- bind Ch3 to R2
- bind DpadUp to Max Speed Increase
- bind DpadDown to Max Speed Decrease
- bind X to Break

There are a number of ways in which each binding can be configured, since different actions will require different parameters based on input.  This document explains (in maybe too much detail) what each parameter does, how it works, and how to consider what values you might need in your case.

## Input Range
Each binding specifies a minimum and maximum range that the value of gamepad events can be.  In most cases, the standard range of an input should be provided for each binding, in order to make full use of the input.  Sometimes, however, it can be useful to provide a custom range, to change how the input's value is interpreted.  

In general, the following ranges are returned by each input:

| Gamepad input                       | Min Value | Max Value |
|-------------------------------------|-----------|-----------|
| Analog Stick                        | -511      | 512       |
| Analog Trigger (brake and throttle) | 0         | 1023      |
| Button                              | 0         | 1         |

> [!TIP]
> Even though the button's output is technically binary, it's still considered a numeric value so that any action can be mapped to any event.


> [!IMPORTANT] The input's default position might not be 0 RPM!
> Consider that the pulse width of the PWM output to an ESC ranges from 1000µs to 2000µs; the halfway point of 0 RPM is 1500µs.  With an analog stick, which ranges from -511 to +512, the default (or neutral) position happens to be 0.  With the analog stick's standard range, the default position corresponds to 0 RPM.
>
> However, consider an analog trigger, where the input ranges from 0 to 1023.  The default position is 0, which will correspond to a pulse width of 1000µs (which is full RPM backwards)!  To tell the ESC to stop the motor, the trigger will have to be *perfectly half-pressed*!
>
> One way to get around this is to double the range.  Instead of specifying the range as 0 to 1023, specify it as -1023 to +1023.  Although this means that half of the specified range will never be met, it does mean that the default position will always be the at the middle of the range!

To demonstrate these concepts, here are a few examples:
- For servo channel 1, it's most common to bind the Ch1 action to the Left Analog Y event.  To make full use of the Y axis' range, `min` should be -512 and `max` should be +512
- Say you had a spinning weapon motor which takes standard PWM servo input on Ch3.  One fun control scheme would be to bind Ch3 to R2 (assuming it's an analog trigger), with `min=-1023` and `max=1023`.  You would have full control over the motor with R2.  (Remember that the range is twice as big as analog triggers provide so that the default position is always in the middle).  
Now, consider what would happen if you made another bind, from Ch3 to *L2*, with `min=-1023` and `max=1023`.  This would provide the same range as the first bind, but since the range is flipped the motor will run backwards!  This allows you to control the rotation *and direction* of the motor, where L2 makes the motor spin one way and R2 makes it spin the other!

## What happens when no controllers are connected?
When the event manager runs each binding, and there aren't any controllers connected, it does one of two things depending on the value of the binding parameter `exec_without_controller`:

### `exec_without_controller` is false
In this case, the bound action simply won't be executed if there isn't any connected controller.  If the action affects some state when it is called, that state will remain unchanged once the controller is disconnected.  For example, if you have an action bound to pull some GPIO pin high in response to a button being pressed, and the button is pressed when the controller disconnects, the action won't be called but the GPIO pin will still be high!

Normally this would be a huge issue for motors; if your controller runs out of battery when the device is in motion, and `exec_without_controller` is false for the motor binding, the motors will just keep going!  However, there's a failsafe in place which means that *all* motors will stop when no controller is connected.  For more info, check the [failsafe documentation](./failsafes.md#kill-motors-when-no-controllers-are-connected).

### `exec_without_controller` is true (default)
In this case, the bound action will be executed irrespective of how many controllers are connected.  Since there's no controller to provide an input value for the action, a default of 0 is assumed.  This is an appropriate value for most cases, but in future versions there might be a way to provide a custom default for each binding.

# Implementation
To implement this, each supported receiver action implements a standard function prototype:
```c++
void actn_something(int32_t value, int32_t min, int32_t max, uint8_t pin);
```
where `value` is the current value of the input, and `min` and `max` are the specified minimum and maximum ranges of the input.  For analog sticks, `min` is -512 and `max` is +512; for buttons `min` is 0 and `max` is 1.  It's up to each action function to process the input value as required (so a PWM channel function would scale `value` linearly to determine the resulting PWM pulse width, whereas a digital function would probably resolve anything that's > `(max/2)` as true, and anything else as false).  As such, action functions should never assume the ranges of the input value, and should always consider the input relative to the provided range.

- there's a cpp file that implements the "event handler"
- on each BP32 update, it iterates over each binding, gets the specified input value, and passes it along with the specified ranges to the specified action
- for specifying events and actions i could
  - pass a std::function<>, ie: you pass the class function itself.  might have issues where functions return different types
  - have an enum or string for each event or action.  would probably make implementing dynamic binding loading easier (if you can deserialise enums)