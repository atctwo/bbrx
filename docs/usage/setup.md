# Setup and Installation
bbrx is based on [Arduino](https://www.arduino.cc/), and is designed for ESP32 microcontroller boards.  This page describes how to get bbrx running on your ESP32 board.

## Building and Uploading bbrx
bbrx is an Arduino sketch, but there are two ways you can build and upload the code:
- using the Arduino IDE that's used with most Arduino projects, or
- using the included [`Makefile`](./Makefile), which makes calls to the [Arduino Command Line Interface (CLI)](https://github.com/arduino/arduino-cli)

### Arduino IDE
This project is designed as an Arduino sketch, for ESP32 boards.  Since it depends on Bluepad32, it has to use a custom BP32 Arduino board definition.  To install this board package, add the following URLs to the "additional board manager URLs" settings entry in the Arduino Preferences dialog:

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json,https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json
```

Then, find the Bluepad32 boards in the board manager, and install the board package.  Once it's installed, select the BP32 version of whichever ESP32 board you're using.

> [!TIP]
> Bluepad32's documentation contains more detailed [documentation](https://bluepad32.readthedocs.io/en/latest/plat_arduino/) on how to do this.

Next, you have to install bbrx's library dependencies from Arduino's library manager.  The current dependencies are:
- [ESP32Servo](https://github.com/madhephaestus/ESP32Servo)
- [SdFat](https://github.com/greiman/SdFat), **version 2.2.2**
- [FastLED](https://github.com/FastLED/FastLED)

Finally, you can build and upload the code as normal.  The sketch is located in the [`bbrx`](../../bbrx/) directory at the root of this repo.

### Arduino CLI
bbrx comes with a [`Makefile`](./Makefile) which performs calls to `arduino-cli` to perform all of the required setup and build commands automatically!  This is my preferred way of building bbrx since you can use any code editor or IDE you want!

1. First, make sure `arduino-cli` is installed (Arduino provide their own instructions [here](https://arduino.github.io/arduino-cli/1.0/installation/)).  
2. Install the required board package and libraries by running `make setup`.  
3. Finally, just run `make` to build and upload the project.