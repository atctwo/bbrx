#include <map>
#include <math.h>
#include <Arduino.h>
#include <FastLED.h>
#include "log.h"
#include "config.h"
#include "status_led.h"

#define LOG_TAG "status"
bool are_leds_setup = false;
CRGB leds[STATUS_NUM_LEDS];

uint8_t current_brightness = STATUS_LED_INIT_BRIGHTNESS;
LED_STATE current_state = LED_OFF;
LED_STATE previous_state = LED_OFF;

int16_t current_pulse_delay = -1;       // flag to indicate the led should be pulsing
uint64_t last_pulse_update = 0;
uint8_t pulse_brightness = current_brightness;
int8_t pulse_step = -1;
uint8_t pulse_divider = 1;

/**
 * A map of each system state to an object which describes what to do with the status led.
 * This object is an std::pair, where the first element is the colour to show, and the second 
 * element is how to pulse the led (time for one period in ms, -1 means don't pulse).
 * 
 * The idea of this map is that the colours to show for each state (and whether or not to pulse
 * the led) are encoded here, so that the update function can be pretty generic.
 */
std::map<LED_STATE, std::pair<CRGB, int16_t>> led_state_settings = {
    {LED_OFF,       {CRGB::Black, -1}},
    {LED_LOADING,   {CRGB::Green, 1000}},
    {LED_IDLE,      {CRGB::Blue,  1000}},
    {LED_CONNECTED, {CRGB::Cyan,  -1}},
    {LED_BRAKE,     {CRGB::Red,   -1}},
};

/**
 * Initialise the LEDs used for showing the status.  Should only be called once at the start of the program.
 */
void leds_setup() {

    if (!are_leds_setup) {

        #ifdef STATUS_LED_ENABLE

            logi(LOG_TAG, "Setting up Status LED");
            logd(LOG_TAG, "%d leds on pin %d, init brightness = %d", STATUS_NUM_LEDS, STATUS_LED_PIN, STATUS_LED_INIT_BRIGHTNESS);

            // enable ws2812 power
            #ifdef STATUS_LED_POWER_PIN
                pinMode(STATUS_LED_POWER_PIN, OUTPUT);
                digitalWrite(STATUS_LED_POWER_PIN, HIGH);
            #endif

            // register LEDs
            FastLED.addLeds<STATUS_LED_TYPE, STATUS_LED_PIN>(leds, STATUS_NUM_LEDS);
            FastLED.setBrightness(STATUS_LED_INIT_BRIGHTNESS);
            leds[0] = CRGB::White;
            FastLED.show();

        #else

            logi(LOG_TAG, "Status LED is disabled");

        #endif

        are_leds_setup = true;
    }


}

/**
 * Specify a new maximum brightness value.
 * 
 * For states which display a solid colour, this determines the brightness.  For states which
 * display a pulsing colour, this determines the maximum brightness the LED reaches.
 * 
 * @param brightness the maximum brightness, from 0 to 255
 */
void leds_set_brightness(uint8_t brightness) {

    #ifdef STATUS_LED_ENABLE

        logi(LOG_TAG, "Updating LED brightness to %d", brightness);

        // set new brightness
        current_brightness = brightness;
        FastLED.setBrightness(brightness);

        // calculate new pulse delay
        current_pulse_delay = (current_pulse_delay == -1) ? -1 : (int16_t) ((float)current_pulse_delay / 250.0);
        if (current_brightness != 0) pulse_divider = (uint8_t) round(256.0 / (float)current_brightness);
        else                         pulse_divider = 0;

    #endif
}

/**
 * @brief Change the LED colour to reflect a change in program state
 * 
 * This function is designed to be called at various points in the program to indicate that it is
 * in some different state (eg: loading, idle, connecting, etc).  Please refer to the `LED_STATE`
 * enum for a complete list of supported states.
 * 
 * Each state will have its own predefined colour and animation settings, which are defined in the
 * `led_state_settings` map in status_led.cpp
 * 
 * @param new_state the state of which to show the relevant LED colour and animation
 */
void leds_set_state(LED_STATE new_state) {

    #ifdef STATUS_LED_ENABLE

        previous_state = current_state;
        current_state = new_state;

        logd(LOG_TAG, "changed state to %d", current_state);

        // read settings from map
        CRGB    col =          led_state_settings[current_state].first;
        int16_t pulse_period = led_state_settings[current_state].second;

        // set new led colour
        leds[0] = col;

        // update pulsing variables
        current_pulse_delay = (pulse_period == -1) ? -1 : (int16_t) ((float)pulse_period / 250.0);
        if (current_brightness != 0) pulse_divider = (uint8_t) round(256.0 / (float)current_brightness);
        else                         pulse_divider = 0;

        // if new state does not require pulsing, reset the pulse variables for next time
        if (current_pulse_delay == -1) {
            FastLED.setBrightness(current_brightness);
            pulse_brightness = 255;
            pulse_step = -1;
        }

        logd(LOG_TAG, "setting led to %d %d %d, pulse period = %d, pulse delay = %d, divider = %d, brightness = %d", col.r, col.g, col.b, pulse_period, current_pulse_delay, pulse_divider, current_brightness);
        FastLED.show();

    #endif
}

/**
 * @brief Similar to `leds_set_state()`, but switches to whatever the previous state was.
 */
void leds_set_state_previous() {
    #ifdef STATUS_LED_ENABLE
        leds_set_state(previous_state);
    #endif
}

uint64_t eee = 0;

/**
 * @brief Function to update the animations of LEDs.  Should be called as frequently as possible.
 */
void leds_update() {

    #ifdef STATUS_LED_ENABLE

        // if pulsing is enabled for the current state
        if (current_pulse_delay != -1) {

            // if the pulse delay has elapsed
            if (millis() - last_pulse_update >= current_pulse_delay) {

                // set led brightness
                if (pulse_divider != 0) {
                    uint8_t aaa = quadwave8(pulse_brightness) / pulse_divider;
                    // logd(LOG_TAG, "b=%d, s=%d, c=%d", pulse_brightness, pulse_step, aaa);
                    FastLED.setBrightness(aaa);
                    FastLED.show();

                    // update brightness variable for next time
                    pulse_brightness += 1;

                    // if (pulse_brightness == 0) {
                    //     logd(LOG_TAG, "%0d (delta %4d)", millis(), millis()-eee);
                    //     eee = millis();
                    // }
                }

                // reset current pulse timer
                last_pulse_update = millis();
            }

        }

    #endif
}