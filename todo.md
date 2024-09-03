# bbrx todo list
This is a list of all the planned features of bbrx.

- [x] ESC input
- [x] Make sure it works with most ESCs
- [x] Git repo
- [x] replace `Servo::write()` with `Servo::writeMicroseconds()` and define min + max pulse width in config.h
- [x] PWM frequency in config.h
- [x] Variable max speed
- [x] Failsafes
- [x] Failsafe Documentation (.md)
- [ ] Feedback (eg: led colour, rumble)
- [ ] On-board WS2812 status
- [x] Dynamic pin config (in config.h for each channel you specify which input to react to somehow)
- [x] make sure that only one event can call an action at a time
- [x] fix deadzones
- [x] add deadzones to other analog inputs
- [x] fix exec without controller being false by default
- [x] Weapon control using L2 / R2
- [x] load bindings from config.h
- [x] add dpad events
- [ ] event repeat timing
- [x] print controller info on connect
- [ ] cumulative bindings
- [x] speed action, which just writes the input directly to `speed_limit`
  - so you could bind speed to the brake input, and a ch1 to the throttle input
- [ ] replace register_binding params with a single struct
- [ ] verbose log level
- [ ] ansi colour codes in output (to differentiate system events and output events) for different log levels
- [ ] fix brake action name spelling error
- [ ] Documentation
- [ ] update outdated info in main readme
- [ ] update outdated info on event implementation
- [ ] document binding creation
  - [ ] deadzones
  - [ ] servo pin limits
- [ ] credits on readme
- [ ] docs index

# Declaring Bindings in config.h
### Phase 1
- update `register_binding()` so that it accepts a `binding` struct object instead of params (since there are a lot of params)
- add default values to `binding` struct, rather than in `register_binding()`
- update default binding registrations to use initialiser lists to init object

### Phase 2
- think of some way to declare bindings in config.h that can be translated / interpreted into an initialiser list
- maybe an initaliser list using dot notation, eg
```c
#define BINDING_COUNT 3
#define BINDINGS_OR_SOMETHING {
  {.action = BBRX_ACTION_HELLO1, .event = BBRX_EVENT_A, .min = 0, .max = 1},
  {.action = BBRX_ACTION_HELLO2, .event = BBRX_EVENT_LX, .min = -512, .max = 512, .pin = 12},
  {.action = BBRX_ACTION_HELLO3, .event = BBRX_EVENT_LY, .min = -512, .max = 512, .pin = 13},
}
```

# Cumulative Binding ideas
- instead of setting the output relative to the input, it has a variable which is used as the output and each on "tick" it adds the input value to the variable
- if the input is 0 / neural, there will be no change in output
- otherwise the output will go up or down
- can be used to make "sticky" outputs (ie: move the analog stick up to increase, down to decrease, neutral to maintain speed)
- in theory could also be used for gyro and accel inputs

# Future Ideas :thinking:
- dynamically load bindings from an SD card

# dpad bitmask

| Bit    | 7 | 6 | 5 | 4 | 3    | 2     | 1     | 0  |
|--------|---|---|---|---|------|-------|-------|----|
| Button |   |   |   |   | left | right | down  | up |