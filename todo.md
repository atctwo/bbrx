# bbrx todo list
This is a list of all the planned features of bbrx.

## Features
- [ ] Feedback (eg: led colour, rumble)
- [ ] event repeat timing
- [ ] cumulative bindings
- [ ] update littlefs make target to read lfs size+offset from partitions.csv
- [ ] pull + build mklittlefs automatically

### Possible Future Ideas
- WebUSB-based website for uploading / downloading / modifying config.yml files from LittleFS

## Documentation
- [ ] getting started page
- developer docs
  - [ ] binding implementation
  - [ ] event manager implementation
  - [ ] how each action is implemented


# Cumulative Binding ideas
- instead of setting the output relative to the input, it has a variable which is used as the output and each on "tick" it adds the input value to the variable
- if the input is 0 / neural, there will be no change in output
- otherwise the output will go up or down
- can be used to make "sticky" outputs (ie: move the analog stick up to increase, down to decrease, neutral to maintain speed)
- in theory could also be used for gyro and accel inputs