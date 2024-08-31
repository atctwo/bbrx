# bbrx
Hello!  This repo contains an Arduino-based receiver for Battlebots (and probably other types of robot or drone), that is controlled using a Bluetooth gamepad instead of a flight controller.

# Background
Many Battlebots are designed to receive commands from standard flight controllers like those you would use with drones or quadcopters.  These controllers communicate with receiver modules using one of many different RF protocols.  The receivers often produce a PWM output in response to the commands sent by the controllers, usually as input to an Electronic Speed Controller (ESC).

I don't have a flight controller, but I have lots of ESP32 boards laying around.  So I decided to make this thing, which replaces the conventional receiver with a microcontroller with Bluetooth running Arduino, and replaces the flight controller with a standard Bluetooth gamepad.

# Features
Progress is being tracked in [todo.md](./todo.md); for now here is a list of completed features:
- outputs servo PWM used by most ESCs
- highly configurable [binding system](./docs/events.md) where any action can be bound to any gamepad input
- speed control
- [failsafes](./docs/failsafes.md)
- configurable deadzones

# Controls
Here is a big list of every output or reaction that bbrx produces to each input on the gamepad:

| Gamepad Event             | Response                         |
|---------------------------|----------------------------------|
| Left Analog Stick, X axis | Controls the output of channel 1 |
| Left Analog Stick, Y axis | Controls the output of channel 2 |

For each PWM channel, the PWM output is set to 1500µs when the stick is neutral, 1000µs when the stick is fully negative, and 2000µs when the stick is fully positive.

# Building
## Arduino IDE
This project is designed as an Arduino sketch, for ESP32 boards.  Since it depends on Bluepad32, it has to use a custom BP32 Arduino board definition.  To install this board package, add the following URLs to the "additional board manager URLs" settings entry:

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json,https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json
```

Then, find the Bluepad32 boards in the board manager, and install the board package.  Once it's installed, select the BP32 version of whichever ESP32 board you're using.

> [!TIP]
> Bluepad32's documentation contains more detailed [documentation](https://bluepad32.readthedocs.io/en/latest/plat_arduino/) on how to do this.

Next, you have to install bbrx's library dependencies from Arduino's library manager.  The current dependencies are:
- [ESP32Servo](https://github.com/madhephaestus/ESP32Servo)

Finally, you can build and upload the code as normal.

## Arduino CLI
bbrx comes with a [`Makefile`](./Makefile) which performs calls to `arduino-cli` to perform all of the required setup and build commands automatically!  First, install the required board package and libraries by running `make setup`.  Next, just run `make` to build and upload the project.