#include <Arduino.h>
#include <Bluepad32.h>
#include "status_led.h"
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
        logi(LOG_TAG, "  - model:   %s", ctl->getModelName().c_str());
        logi(LOG_TAG, "  - battery: %d%%", (ctl->battery() / 255) * 100);

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

        // set status led
        leds_set_state(LED_CONNECTED);

    }
    else {
        logw(LOG_TAG, "Attempted to connect to new controller, but couldn't because already connected to a different one");

        // disconnect the new gamepad so it knows it's not actually doing anything
        ctl->disconnect();
    }

}

/**
Callback for when a controller is disconnected
*/
void controller_callback_disconnected(ControllerPtr ctl) {

    if (ctl == controller) {
        logi(LOG_TAG, "Controller disconnected!");
        controller = nullptr;

        // set status led
        leds_set_state(LED_IDLE);
    } else {
        logw(LOG_TAG, "Mysterious unknown gamepad disconnected");
    }

}

void controller_setup() {
    logi(LOG_TAG, "Setting up Bluepad32");
    logi(LOG_TAG, "BP32 version: %s", BP32.firmwareVersion());
    BP32.setup(&controller_callback_connected, &controller_callback_disconnected);
    logi(LOG_TAG, "Listening for controllers...");
}

void controller_handle(std::function<void(ControllerPtr controller)> callback) {

    // update bluepad32
    BP32.update();

    // call callback
    // note: callback must check if controller is nullptr!!!
    callback(controller);

}

bool controller_connected() {

    // this should return true if >=1 controller is connected.  bbrx only supports
    // one controller at a time for now, so really this function returns true if
    // The Controller is connected

    return (controller != nullptr);

}