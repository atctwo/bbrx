// #include <cstdint>
#include <Arduino.h>
#include "controllers.h"
#include "event_manager.h"
#include "status_led.h"
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
    #ifdef GIT_HASH
        logi(LOG_TAG, "commit:      %s", GIT_HASH);
    #endif
    logi(LOG_TAG, "");

    // setup the status led
    leds_setup();
    leds_set_state(LED_LOADING);

    // this will load the user config from the configured filesystem
    // (or will just leave the defaults if this fails).
    // either way, once this function finishes, a bindset will be set up but not initialised
    load_config();

    // setup controller library
    controller_setup();

    // setup event manager and hardware needed by actions
    event_manager_setup();

    // finally, initialise each registered binding
    for (auto b : bindings) {
        initialise_binding(b);
    }

    // done!  now set the status led to idle
    leds_set_state(LED_IDLE);

    logi(LOG_TAG, "Setup complete!");
}

void loop() {

    // parse controller input and perform bound actions
    event_manager_update();

    // let the status led update
    leds_update();

}
