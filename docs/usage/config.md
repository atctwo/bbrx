# bbrx configuration
One of the design goals of bbrx is versatility - the idea is that it can used in multiple different situations, and can be _adapted_ to work best for each one.  To that end, bbrx is very configurable.  Much of the program's functionality can be specified using a YAML file called **`config.yml`**.  This document discusses both how bbrx finds this file, and explains all the configuration properties the file can have.

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
