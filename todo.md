# bbrx todo list
This is a list of all the planned features of bbrx.

## Features
- [ ] Feedback (eg: led colour, rumble)
- [x] On-board WS2812 status
- [ ] event repeat timing
- [ ] cumulative bindings
- [x] GPIO action
- [ ] CI to test pushes to github
- [x] Load config from file
- [x] SD support
- [x] load deadzones from config.yml
- [ ] update littlefs make target to read lfs size+offset from partitions.csv
- [ ] pull + build mklittlefs automatically

## Documentation
- usage docs
  - [x] update outdated info in main readme
  - [x] update outdated info on event implementation
  - [x] document binding creation
    - [x] deadzones
    - [x] servo pin limits
    - [x] how to write config.yml files 
  - [x] document config file loading approach
  - [x] move docs into usage folder
- developer docs
  - [ ] binding implementation (and also i guess how to write _default_ bindings)
  - [ ] event manager implementation
  - [ ] how each action is implemented
- [x] credits on readme
- [x] docs index
- [x] tidy up comments on config.h


# Cumulative Binding ideas
- instead of setting the output relative to the input, it has a variable which is used as the output and each on "tick" it adds the input value to the variable
- if the input is 0 / neural, there will be no change in output
- otherwise the output will go up or down
- can be used to make "sticky" outputs (ie: move the analog stick up to increase, down to decrease, neutral to maintain speed)
- in theory could also be used for gyro and accel inputs

# Load Config From File ideas
- instead of bindings and other non-system stuff being defined in config.h, it would be cool to load from a file
- everything would probably be defined in a YAML or JSON file
- the FS code would operate on the `FS` object so that it can be used with SD, SPIFFS, LittleFS, etc
  - could support boards with an SD card reader, but i think a flash-based thingy (like littlefs) would probably work most of the time
  - hmmm maybe on boot, bbrx tries to detect an SD card, if there is one it uses that, otherwise it falls back to littlefs, otherwise it falls back to defaults
  - hmmmmmm the SD card pinout might be different for different boards, so the SD card pins would probably have to be #defined anyway.  meaning you might need to make different builds for each board
  - the only issue is that uploading flash FS images is really hard.  i think it can be done with an arduino IDE plugin, but it would be doable to write a makefile target with `mklittlefs` and `esptool`
  - could maybe make a little GUI to generate + flash images (and maybe even generate config files)
- either way, all the variables remain in config.h (as variables, not defines).  there's a file called config.cpp or something, which has all the code for opening and parsing config.txt.  when a config file is parsed, it overwrites all the (specified) variables in config.h with the new values
  - will probably increase static memory usage :(
  - bindings will probably have to be stored in a vector, which is basically cleared when new bindings are loaded
  - this feels a bit inefficient but any kind of runtime config loading cannot use defines to work.  it all has to be variables
  - i guess it allows for the config to be reloaded at runtime
- considered having a littlefs make target be part of the all target, but what happens when littlefs doesn't exist for some reason?  also arduino ide


## LittleFS config update flow
- so the ESP32 has a newly flashed copy of bbrx on it.  how do you upload your config file?
- using makefile
  - there's a make target which uploads the file.  you have to point it to the location of your config file, it calls `mklittlefs` and builds an image, then uploads it
  - eg: `make upload_config /home/alice/projects/config.txt`
- using a GUI
  - i guess you would just use a file open dialog to select the config file and it would run `make upload_config`
  - or might just run `mklittlefs` directly to not have to depend on `make` being installed
- using an SD card
  - you put the file on the root of the SD card.  bbrx detects that (1) there's an SD card, and (2) there's a config.txt