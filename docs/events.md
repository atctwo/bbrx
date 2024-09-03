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
Each binding specifies a minimum and maximum range that the value of gamepad events can be.  In most cases, the standard range of an input should be provided for each binding, in order to make full use of the input.  The range of values returned by each input can be found in the [complete event list](./action_event_list.md#events-bb_event).  Sometimes, however, it can be useful to provide a custom range, to change how the input's value is interpreted.  

> [!TIP]
> Even though buttons' outputs are technically binary, they're still considered numeric values so that any action can be mapped to any event.


> [!IMPORTANT] 
> **The input's default position might not be 0 RPM!**
> 
> Consider that the pulse width of the PWM output to an ESC ranges from 1000µs to 2000µs; the halfway point of 0 RPM is 1500µs.  With an analog stick, which ranges from -511 to +512, the default (or neutral) position happens to be 0.  With the analog stick's standard range, the default position corresponds to 0 RPM.
>
> However, consider an analog trigger, where the input ranges from 0 to 1023.  The default position is 0, which will correspond to a pulse width of 1000µs (which is full RPM backwards)!  To tell the ESC to stop the motor, the trigger will have to be *perfectly half-pressed*!
>
> One way to get around this is to double the range.  Instead of specifying the range as 0 to 1023, specify it as -1023 to +1023.  Although this means that half of the specified range will never be met, it does mean that the default position will always be the at the middle of the range!

To demonstrate these concepts, here are a few examples:
- For servo channel 1, it's most common to bind the Ch1 action to the Left Analog Y event.  To make full use of the Y axis' range, `min` should be -512 and `max` should be +512
- Say you had a spinning weapon motor which takes standard PWM servo input on Ch3.  One fun control scheme would be to bind Ch3 to the throttle trigger (on R2), with `min=-1023` and `max=1023`.  You would have full control over the motor with R2.  (Remember that the range is twice as big as analog triggers provide so that the default position is always in the middle).  
  - Now, consider what would happen if you made another bind, from Ch3 to *the brake input (on L2)*, with `min=-1023` and `max=1023`.  This would provide the same range as the first bind, but since the range is flipped the motor will run backwards!  This allows you to control the rotation *and direction* of the motor, where L2 makes the motor spin one way and R2 makes it spin the other!
  - what happens when you press both triggers at the same time?  check out the section on [action claiming](#action-claiming) to find out!

## What happens when no controllers are connected?
When the event manager runs each binding, and there aren't any controllers connected, it does one of two things depending on the value of the binding parameter `exec_without_controller`:

### `exec_without_controller` is false
In this case, the bound action simply won't be executed if there isn't any connected controller.  If the action affects some state when it is called, that state will remain unchanged once the controller is disconnected.  For example, if you have an action bound to pull some GPIO pin high in response to a button being pressed, and the button is pressed when the controller disconnects, the action won't be called but the GPIO pin will still be high!

Normally this would be a huge issue for motors; if your controller runs out of battery when the device is in motion, and `exec_without_controller` is false for the motor binding, the motors will just keep going!  However, there's a failsafe in place which means that *all* motors will stop when no controller is connected.  For more info, check the [failsafe documentation](./failsafes.md#kill-motors-when-no-controllers-are-connected).

### `exec_without_controller` is true (default)
In this case, the bound action will be executed irrespective of how many controllers are connected.  Since there's no controller to provide an input value for the action, a default of 0 is assumed.  This is an appropriate value for most cases, but in future versions there might be a way to provide a custom default for each binding.

## Action Claiming
What happens when an action is triggered by two separate simultaneous input events?  Without action claiming, the bindings for both events will run at the same time, so each action will be run twice, against both inputs.  This could be a problem for things like motors; if one binding is telling the motor to run at 100 RPM and one is telling it to run at -50 RPM, what happens?

To resolve this problem, actions are "claimed" by the first binding that runs which calls that action.  That is, actions are allowed to be executed by inputs on a first-come-first-serve basis.  When an input event registers with a non-default value (meaning that the control is assumed to have been moved out of the neutral position), the first binding to be executed for each combination of action and pin will claim that combination.  As soon as the input returns to the default value, the claim will be removed so future bindings can call the action.

The result of this is that no two inputs can control an action *at the same time*, but it means that two inputs can control the same action at different times.  If you had trigger and brake bound to a single servo channel, and you pressed the trigger, the brake would do nothing (at least for the motor) until you release the trigger.  While the trigger is pressed, the trigger-servo binding has a claim to that servo channel on that pin, and nothing else can affect it until the claim is lost.

> [!TIP] 
> **Ignoring Claims**
> 
> In some cases it might be useful for a binding to always run, irrespective of who has claimed the action (if anyone).  In these cases, each binding has a flag called `ignore_claims`; if this is set, then when the binding is run it will just ignore any claims made on the bound action.  Keep in mind that this binding will then make it's own claim on the action, if one hasn't already been made.

> [!NOTE] 
> **Combination of Action and Pin**
> 
> Claims aren't just made for each action.  Since all servo output is performed by the `BB_ACTION_SERVO` action, if this action were to be claimed on it's own then no other servos could run at all!  As such, claims are made on the combination of action and pin.  So, when a binding is made to a servo channel, claims are actually made for the `BB_ACTION_SERVO` action *on the specified pin*, meaning that `BB_ACTION_SERVO` actions can still run on other pins.  This allows multiple claims to be made for the same action, with the pin number acting as a sort of index.

> [!NOTE] 
> **Implementation**
> 
> This feature is implemented using the following logic:
> - when a binding is executed, it checks if the action has been claimed.  if it has, then the binding does nothing.  
> - if (1) the action has not been claimed, (2) the action has been claimed for the current binding, or (3) the ignore claims flag is set for the current binding
>   - if the event value is *not* the default, it sets the claimed flag of the action
>   - if the event value *is* the default, it clears the claimed flag
>   - either way, the action will be executed as normal (pending [`exec_without_controller`](#what-happens-when-no-controllers-are-connected) and other conditions)

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