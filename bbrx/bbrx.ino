// #include <cstdint>
#include <Arduino.h>
#include "controllers.h"
#include "event_manager.h"
#include "log.h"
#include "config.h"

#define LOG_TAG "main"

void setup() {

    // setup serial
    Serial.begin(115200);
    delay(1500);
    logi(LOG_TAG, "\n=== bbrx! ===");
    logi(LOG_TAG, "version:     %s", VERSION_STRING);
    logi(LOG_TAG, "build time:  %s %s", __DATE__, __TIME__);
    logi(LOG_TAG, "");

    // setup controller library
    controller_setup();

    // setup event manager and hardware needed by actions
    event_manager_setup();


    // main motor control bindings
    register_binding(BB_ACTION_SERVO, BB_EVENT_ANALOG_LY, -512, 512, PIN_OUT_CH1);
    register_binding(BB_ACTION_SERVO, BB_EVENT_ANALOG_LX, -512, 512, PIN_OUT_CH2);

    // weapon control bindings
    // register_binding(BB_ACTION_SERVO, BB_EVENT_ANALOG_BRAKE,    -1024,  1024, PIN_OUT_CH1);
    // register_binding(BB_ACTION_SERVO, BB_EVENT_ANALOG_THROTTLE,  1024, -1024, PIN_OUT_CH1);

    // brake and speed control
    register_binding(BB_ACTION_BREAK, BB_EVENT_BTN_A, 0, 1);
    register_binding(BB_ACTION_SPEED_UP, BB_EVENT_BTN_B, 0, 1);
    register_binding(BB_ACTION_SPEED_DOWN, BB_EVENT_BTN_X, 0, 1);
}

void loop() {

    // parse controller input and perform bound actions
    event_manager_update();

}
