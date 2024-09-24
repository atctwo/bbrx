# bbrx configuration
One of the design goals of bbrx is versatility - the idea is that it can used in multiple different situations, and can be _adapted_ to work best for each one.  To that end, bbrx is very configurable.  Much of the program's functionality can be tweaked using a YAML file called **`config.yml`**.  This document discusses both how bbrx finds this file, and explains all the configuration properties the file can have.

The most important thing you can configure are **bindings** - these are basically how bbrx knows what to do when certain input events occur.  For more info on how bindings work, check out [**Events and Binding**](events.md).

# Config File Loading Process
When bbrx boots up, one of the first things it does is search for `config.yml`.  It searches a couple of places (which take the form of filesystems) with a predefined priority (ie: it checks each place in order until it finds one with which `config.yml` can be read).  If none of the configured places can be read, or none of them have (a valid) `config.yml`, then the default values provided in [`config.h`](../../bbrx/config.h) and [`config.cpp`](../../bbrx/config.cpp) will be used.

Currently, the following filesystems are supported (listed in order of which is checked first):
- external FAT32-formatted SD Card
- LittleFS partition on the ESP32's flash memory
- default values

The following subsections go into detail about how to setup each filesystem.  Also, each filesystem can individually be enabled or disabled by (un)commenting the relevant `CONFIG_ENABLE_*` define in [`config.h`](../../bbrx/config.h).

## SD Card
The first location which is searched is an SD card.  For this location, bbrx will first try to initialise the SD card; it will attempt to communicate with it using SPI on the pins defined in [`config.h`](../../bbrx/config.h).  If it fails to initialise it moves on to the next method.  If it succeeds, then it will try to read `config.yml` from the root of the SD.

Because most ESP32 boards don't have an SD card slot, this location is actually disabled by default.  To make use of it, go to `config.h` and uncomment the line
```c++
//#define CONFIG_ENABLE_SD
```
Make sure the `CONFIG_SD_PIN_*` defines are all set to match the pins that the SD card uses on your particular board.

## LittleFS
[LittleFS](https://github.com/littlefs-project/littlefs) is a small, lightweight filesystem stored on the flash memory which the ESP32 loads its code off of.  It has the benefit of not requiring any external parts or devices, since all ESP32s will have a flash chip.  A downside is that it takes up memory that could be used for storing the program, but bbrx isn't that big so it's not really an issue here.

The flash memory on an ESP32 is split up into a series of "partitions", which define what type of data is stored on the flash and where.  When you upload the program onto an ESP32, the code is actually loaded into one of the `app` partitions in the flash.  LittleFS filesystems are stored on their own partition.  (The partition layout for bbrx is actually defined in a [custom partition file](../../bbrx/partitions.csv)).  So, to upload to the `app` partition you perform the "upload" action - how do you upload files to the LittleFS partition?  Specifically how do you upload `config.yml`?

First off, there is a folder called [`bbrx/data`](../../bbrx/data/).  When you "upload" a LittleFS filesystem, the contents of this folder are packaged into an "image" file, which is then written to the correct position in the flash memory.  This is where your `config.yml` lives!  There are two ways to build and upload an image, depending on which way you are using to build bbrx (or whichever you find easier), which are described in the next sections.

Before uploading the LittleFS image, please make sure you have updated [`bbrx/data/config.yml`](../../bbrx/data/config.yml) with your specific config.  Also note that when using LittleFS, you will have to re-upload the image each time you want to change or update the config.

### Uploading LittleFS image using Arduino IDE
There are a couple of plugins which can be used to create and upload LittleFS images, for example Earle F. Philhower's [arduino-littlefs-upload](https://github.com/earlephilhower/arduino-littlefs-upload), or me-no-dev's [Arduino ESP32 filesystem uploader](https://github.com/me-no-dev/arduino-esp32fs-plugin) (which can upload multiple FS types like SPIFFS).  Each of these expect the files to uploaded to be in the `data` directory, so either should work with bbrx.

The documentation for each tool will describe how to install the tool, as well as how to upload files with it, so please refer to the appropriate docs for further usage info!

### Uploading LittleFS image using Makefile
The Makefile which is used to build bbrx also has a target specifically for building and uploading the LittleFS image.  To invoke it, just run `make littlefs`.  This will take the files in `bbrx/data` and package them into a LFS image, then upload them.

This target depends on a couple of external programs:
- to build the image it uses [`mklittlefs`](https://github.com/earlephilhower/mklittlefs).  By default the Makefile expects `mklittlefs` to be available in the PATH environment variable, but you can specify a custom location by setting the `MKLITTLEFS_PATH` variable (eg: `make littlfs MKLITTLEFS_PATH=/usr/bin/mklittlefs`)
- to upload the image, it uses [`esptool`](https://github.com/espressif/esptool).  You can specify the path to the `esptool` Python file by setting the `ESPTOOL_PATH` variable, but by default it tries to locate the latest version bundles with the ESP32 Arduino core, so in most cases it should work without providing a path yourself.

# `config.yml` reference
This section of this document explains each of the things you can configure using `config.yml`.  [Example config files](../../extras/configs/) file are provided in the `extras` directory of this repository.

## Deadzones and Beefzones
Both of these can be specified in `config.yml`.  They each have their own top-level objects: `deadzones` and `beefzones` respectively.  Both them support the same set of integer-type keys for specifying zones for each analog input:

- `lx`, `ly`, `rx`, `ry` are zones for the X and Y axes of the left and right analog sticks
- `brake` and `throttle` are zones for the brake trigger (L2) and throttle trigger (R2)

All of these keys are optional, and default values are specified in [`config.cpp`](../../bbrx/config.cpp).

For example:
```yaml
deadzones:
  ly: 32
  lx: 32
  ry: 32
  rx: 32
  brake: 64
  throttle: 64

beefzones:
  ly: 500
  lx: 500
  ry: 500
  rx: 500
  brake: 1000
  throttle: 1000
```

For more info on what deadzones and... beefzones.. are, please check out their docs on the [Events and Binding](events.md#deadzones-and-beefzones) page.

## Bindings
The heart of bbrx, bindings are expressed as a list of objects under the `bindings` top-level key.  The keys / properties that each object can contain are listed below:

| Property                      | Required?                            | Description                                                        | Section to Reference                                                                               |
|-------------------------------|--------------------------------------|--------------------------------------------------------------------|----------------------------------------------------------------------------------------------------|
| `action`                      | yes                                  | Which action to call                                               |                                                                                                    |
| `event`                       | yes                                  | Which event to call the action as a result of                      |                                                                                                    |
| `min`                         | yes                                  | Minimum value of the input range                                   | [Input Range](events.md#input-range)                                                                        |
| `max`                         | yes                                  | Maximum value of the input range                                   | [Input Range](events.md#input-range)                                                                        |
| `default_value`               | no                                   | Default value to assume when no controller is connected            |                                                                                                    |
| `pin`                         | no (unless the action has an output) | Which pin to produce the output on                                 |                                                                                                    |
| `exec_without_controller`     | no                                   | Whether to execute the binding when no controller is connected     | [What happens when no controllers are connected?](events.md#what-happens-when-no-controllers-are-connected) |
| `ignore_claims`               | no                                   | Whether to ignore claims made on an action-pin combination         | [Action Claiming](events.md#action-claiming)                                                                |
| `conditionals`                | no                                   | Events which must evaluate as true for the action to occur         | [Conditional Events](events.md#conditional-events)                                     |
| `conditional_min`             | no                                   | Minimum value of the range of inputs for conditional events        | [Conditional Events](events.md#conditional-events)                                     |
| `conditional_max`             | no (default=1)                       | Maximum value of the range of inputs for conditional events        | [Conditional Events](events.md#conditional-events)                                     |
| `condiitonal_noexec`          | no                                   | Maximum value of the range of inputs for conditional events        | [Conditional Events](events.md#conditional-events)                                     |

For more info on what each of these actually do and how they work, check out their relevant sections in [the event system docs](events.md)!

Please also note that the values for each key are expected to be of certain data types:
- `action` should be [a supported receiver action](action_event_list.md#actions-bb_action)
- `event` should be [a supported gamepad event](action_event_list.md#events-bb_event)
- `min` and `max` should be integers
- `pin` should be an integer that represents a pin on the ESP32
  - also check the reference for the specific action to make sure it works with the specified pin!
- `default_value` should also be an integer (ideally between `min` and `max`)
- `exec_without_controller` and `ignore_claims` should be boolean (`true` or `false`)
- `conditionals` should either be a supported gamepad event, or a sequence / list of events
- `conditional_min` and `conditional_max`, like regular `min` and `max`, should be integers
- `conditional_noexec` should be boolean

If any property does not match it's expected data type, it won't be included in the binding definition.  If any required properties are missing or fail to parse, then the entire binding will not be registered.

### Example Bindings
#### Basic ESC Control with left analog stick
This example assumes you have an ESC which takes in two servo PWM channels to control the speed and direction of a few motors.  It binds the Y axis of the left analog stick to pin 12 ("channel 1") and the X axis to pin 13 ("channel 2"), allowing you to use the left stick to control the speed and direction of the motors.

```yaml
bindings:
# servo channel 1
- action: BB_ACTION_SERVO
  event: BB_EVENT_ANALOG_LY
  min: -512
  max: 511
  pin: 12

# servo channel 2
- action: BB_ACTION_SERVO
  event: BB_EVENT_ANALOG_LX
  min: 511
  max: -512
  pin: 13
```

#### Analog Speed Control
This binding provides a way to manually set the speed separately from the direction.  With most ESCs, you can use `BB_ACTION_SERVO` action with two output channels to determine both the direction and speed of the motors on it's own.  However it might be useful in some cases to be able to control the speed separately (think about how in a car, the steering wheel controls direction, and the accelerator controls speed).

This binds the throttle (R2) directly to the speed control variable, so that when the throttle is neutral the speed is set to zero.  Even if the servo action is bound, nothing will happen.  When the throttle is fully pressed, the speed is set to it's max value (based on the min and max ESC PWM values), even if there's no input to the servo action.  This makes the throttle act kind of like an accelerator!

Keep in mind that this somewhat conflicts with the `BB_ACTION_SPEED_UP` and `BB_ACTION_SPEED_DOWN` actions.  While these actions will still work, `BB_ACTION_SPEED_SET` will just override the speed variable with the continuous analog input, so the first two actions won't really seem to do anything.

```yaml
bindings:
# analog speed control
- action: BB_ACTION_SPEED_SET
  event: BB_EVENT_ANALOG_THROTTLE
  min: 1023
  max: 0
```

#### Conditional Speed Control
This example provides speed control bindings, where D-Pad Up is bound to Speed Up, and D-Pad down is bound to Speed Down.  However, both bindings are gated behind the START button; that is, they will only work when START is held down.

```yaml
bindings:
- action: BB_ACTION_SPEED_UP
  event: BB_EVENT_DPAD_UP
  conditionals: BB_EVENT_BTN_START
  min: 0
  max: 1

- action: BB_ACTION_SPEED_DOWN
  event: BB_EVENT_DPAD_DOWN
  conditionals: BB_EVENT_BTN_START
  min: 0
  max: 1
```