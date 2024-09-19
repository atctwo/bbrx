CLI           	:= arduino-cli
FQBN          	:= esp32-bluepad32:esp32:adafruit_itsybitsy_esp32
BOARD         	:= esp32-bluepad32:esp32
SKETCH_NAME   	:= bbrx
SERIAL_PORT   	:= /dev/ttyACM1
SERIAL_CONFIG 	:= 115200
GIT_HASH      	:= $(shell git rev-parse --short HEAD)
BUILD_PATH    	:= ./build
CLI_BUILD_PATH 	:= ${BUILD_PATH}/arduino/

ESPTOOL_PATH_A  	:= $${HOME}/.arduino15/packages/esp32/tools/esptool_py
ESPTOOL_PATH_B  	:= $(shell find ${ESPTOOL_PATH_A} -maxdepth 1 -type d | sort -V | tail -1)
ESPTOOL_PATH    	:= ${ESPTOOL_PATH_B}/esptool.py
MKLITTLEFS_PATH 	:= mklittlefs
LFS_IMAGE_SIZE  	:= 0x180000
LFS_IMAGE_OFFSET	:= 0x670000
LFS_IMAGE_PATH      := ${BUILD_PATH}/littlefs/lfs.bin
LFS_DATA_PATH       := ./bbrx/data

BOARD_PKG_ESP32 := https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
BOARD_PKG_BP32  := https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json

# default target to build and upload code
all: build upload monitor

# test since i'm using this to learn how to write makefiles
test:
	@echo "bbrx build test"

# install dependancies
setup:
	@$(CLI) core install $(BOARD) --additional-urls $(BOARD_PKG_ESP32),$(BOARD_PKG_BP32)
	@$(CLI) lib install ESP32Servo
	@$(CLI) lib install SdFat@2.2.2
	@$(CLI) lib install FastLED

# build sketch using arduino-cli
build:
	@$(CLI) compile --fqbn $(FQBN) --build-property build.extra_flags+=-DGIT_HASH=\"${GIT_HASH}\" --build-path ${CLI_BUILD_PATH} $(SKETCH_NAME)

# upload sketch using arduino-cli
upload:
	@$(CLI) upload -p $(SERIAL_PORT) --fqbn $(FQBN) --input-dir ${CLI_BUILD_PATH} $(SKETCH_NAME)

# start the serial monitor
monitor:
	@$(CLI) monitor -p $(SERIAL_PORT) --config $(SERIAL_CONFIG) -b $(FQBN)

# build and upload the littlefs image
littlefs:
	@printf "Building LittleFS image...\n"
	@$(MKLITTLEFS_PATH) -c ${LFS_DATA_PATH} -s ${LFS_IMAGE_SIZE} ${LFS_IMAGE_PATH}
	@printf "\nUploading filesystem\n"
	@python ${ESPTOOL_PATH} --port ${SERIAL_PORT} write_flash ${LFS_IMAGE_OFFSET} ${LFS_IMAGE_PATH}

.PHONY: all build