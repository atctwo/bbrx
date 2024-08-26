#include <Arduino.h>
#include <Bluepad32.h>
#include "log.h"
#include "config.h"

#define LOG_TAG "controller"

ControllerPtr controller;

/**
Callback for when a new controller is connected
*/
void controller_callback_connected(ControllerPtr ctl) {

    // if no other controller is connected
    if (controller == nullptr) {

        // store pointer to controller
        controller = ctl;

        // print controller info
        logi(LOG_TAG, "Connected to a controller!");

        ControllerProperties properties = ctl->getProperties();
        logd(LOG_TAG,
            "BTAddr: %02x:%02x:%02x:%02x:%02x:%02x, VID/PID: %04x:%04x, "
            "flags: 0x%02x",
            properties.btaddr[0], properties.btaddr[1], properties.btaddr[2],
            properties.btaddr[3], properties.btaddr[4], properties.btaddr[5],
            properties.vendor_id, properties.product_id, properties.flags
        );

        // set LED colour
        ctl->setColorLED(0x00, 0xCE, 0xD1);

    }
    else {
        logw(LOG_TAG, "Attempted to connect to new controller, but couldn't because already connected to a different one");
    }

}

/**
Callback for when a controller is disconnected
*/
void controller_callback_disconnected(ControllerPtr ctl) {

    if (ctl == controller) {
        logi(LOG_TAG, "Controller disconnected!");
        controller = nullptr;
    } else {
        logw(LOG_TAG, "Mysterious unknown gamepad disconnected");
    }

}

void controller_setup() {
    logi(LOG_TAG, "Setting up Bluepad32");
    logi(LOG_TAG, "BP32 version: %s", BP32.firmwareVersion());
    BP32.setup(&controller_callback_connected, &controller_callback_disconnected);
}

void controller_handle(std::function<void(ControllerPtr controller)> callback) {

    // update bluepad32
    BP32.update();

    // if controller is connected
    if (controller != nullptr) {

        // call callback
        callback(controller);

    }

}