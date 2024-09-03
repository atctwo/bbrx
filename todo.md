# bbrx todo list
This is a list of all the planned features of bbrx.

## Features
- [ ] Feedback (eg: led colour, rumble)
- [ ] On-board WS2812 status
- [ ] event repeat timing
- [ ] cumulative bindings

## Documentation
- [ ] update outdated info in main readme
- [ ] update outdated info on event implementation
- [ ] document binding creation
  - [ ] deadzones
  - [ ] servo pin limits
- [ ] credits on readme
- [ ] docs index


# Cumulative Binding ideas
- instead of setting the output relative to the input, it has a variable which is used as the output and each on "tick" it adds the input value to the variable
- if the input is 0 / neural, there will be no change in output
- otherwise the output will go up or down
- can be used to make "sticky" outputs (ie: move the analog stick up to increase, down to decrease, neutral to maintain speed)
- in theory could also be used for gyro and accel inputs

# Future Ideas :thinking:
- dynamically load bindings from an SD card