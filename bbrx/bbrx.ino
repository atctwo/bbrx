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
    for (auto b : bindings) {
        initialise_binding(b);
    }
}

void loop() {

    // parse controller input and perform bound actions
    event_manager_update();

}
