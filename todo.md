# bbrx todo list
This is a list of all the planned features of bbrx.

- [x] ESC input
- [x] Make sure it works with most ESCs
- [x] Git repo
- [x] replace `Servo::write()` with `Servo::writeMicroseconds()` and define min + max pulse width in config.h
- [x] PWM frequency in config.h
- [ ] Variable max speed
- [ ] Failsafes
- [ ] Failsafe Documentation (.md)
- [ ] Feedback (eg: led colour, rumble)
- [ ] On-board WS2812 status
- [x] Dynamic pin config (in config.h for each channel you specify which input to react to somehow)
- [ ] make sure that only one event can call an action at a time
- [x] fix deadzones
- [ ] add deadzones to other analog inputs
- [ ] fix max speed control
- [ ] Weapon control using L2 / R2
- [ ] Documentation

# Future Ideas :thinking:
- dynamically load bindings from an SD card