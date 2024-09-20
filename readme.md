# bbrx
Hello!  This is an Arduino-based receiver for radio-controlled devices like drones or quadcopters or battlebots, except instead of using the type of RF signals a flight controller uses, *bbrx uses any Bluetooth gamepad*.

# Background
This project started as a way to control a Battlebot I was building.  Many Battlebots are designed to receive commands from standard flight controllers like those you would use with drones or quadcopters.  These controllers communicate with receiver modules using one of many different RF protocols.  The receivers often produce a PWM output in response to the commands sent by the controllers, usually as input to an Electronic Speed Controller (ESC).

I didn't a flight controller, but I had lots of ESP32 boards laying around and a PlayStation 5 controller.  So I decided to make this thing, which replaces the conventional receiver with a microcontroller with Bluetooth running Arduino, and replaces the flight controller with a standard Bluetooth gamepad.

# Features
Progress is being tracked in [todo.md](./todo.md); for now here is a list of completed features:
- outputs servo PWM used by most ESCs
- highly configurable [binding system](./docs/usage/events.md) where any action can be bound to any gamepad input
- bindings and other [configuration settings](./docs/usage/config.md) can be loaded from an SD card or a LittleFS partition in flash
- real-time speed control actions
- [failsafes](./docs/usage/failsafes.md) to prevent your device from going out of control
- configurable inner and outer deadzones for analog inputs
- support for a cool RGB status LED to show program state

# Documentation
Documentation for all the features of bbrx, including how they inter-relate and how to configure _everything_, is provided in the [**`docs`**](./docs/) directory in the root of this repository.  These docs explain each feature in detail, and aim to provide a complete picture of how all the parts of bbrx operate.

# Default Controls
As mentioned above, bbrx supports dynamic binding of receiver actions to gamepad events.  This allows users to set up their own control schemes specific to their devices.  Bindings can be loaded from an SD card or from flash memory, but in the case where neither of these have a valid config file, bbrx includes a "default" mapping which should cover most applications.  The default bindings are as follows:

| Gamepad Input        | Action                      |
|----------------------|-----------------------------|
| Left Stick X axis    | Servo Channel 1             |
| Left Stick Y axis    | Servo Channel 2             |
| Throttle (on L2)     | Servo Channel 3 (forwards)  |
| Brake (on R2)        | Servo Channel 3 (backwards) |
| A (xbox)             | Brake (kills all motors)    |
| B (xbox), D-pad up   |Max speed up                 |
| X (xbox), D-pad down | Max speed down              |

# Credits
bbrx is built on top of a number of other open source projects!
- [Arduino](https://www.arduino.cc/), of course, and the [ESP-IDF](https://github.com/espressif/esp-idf) SDK and the [Arduino core for ESP32](https://github.com/espressif/arduino-esp32)
- Earle F Philhower III's [mklittlefs](https://github.com/earlephilhower/mklittlefs) tool is used for building LittleFS images
- Ricardo Quesada's [Bluepad32](https://github.com/ricardoquesada/bluepad32) is responsible for all of the Bluetooth gamepad communication
- [ESP32Servo](https://github.com/madhephaestus/ESP32Servo), which implements the Arduino Servo API on the ESP32
- [fkYAML](https://github.com/fktn-k/fkYAML) by ftkn-k is used for parsing the `config.yml` file
- Bill Greiman's [SdFat](https://github.com/greiman/SdFat) library is used for SD card access