#pragma once

/**
 * @brief An enum to represent the main states of bbrx's execution lifecycle, as represented by the status LED
 */
enum LED_STATE {
    LED_OFF,        // led is off
    LED_LOADING,    // initial boot stage, config loading
    LED_IDLE,       // bbrx is ready but not connected to any gamepad
    LED_CONNECTED,  // bbrx is connected and listening to input
    LED_BRAKE       // the brake is enabled
};

void leds_setup();
void leds_set_state(LED_STATE new_state);
void leds_set_state_previous();
void leds_update();