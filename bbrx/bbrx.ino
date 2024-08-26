// #include <cstdint>
#include <Arduino.h>
#include <ESP32Servo.h>
#include "controllers.h"
#include "log.h"
#include "config.h"

#define LOG_TAG "main"

Servo OutputCh1;
Servo OutputCh2;
int16_t speed_limit = 60; // this is the amount by which the top speed (forwards + backwards) is reduced; when this is 0, max speed is 180

void setup() {

    // setup serial
    Serial.begin(115200);
    logi(LOG_TAG, "\n=== bbrx! ===");
    logi(LOG_TAG, "version:     %s", VERSION_STRING);
    logi(LOG_TAG, "build time:  %s %s", __DATE__, __TIME__);
    logi(LOG_TAG, "");

    // setup controller library
    controller_setup();

    // setup servo pwm
    ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	OutputCh1.setPeriodHertz(490);    // standard 50 hz servo
    OutputCh1.attach(PIN_OUT_CH1, 1000, 2000);
    OutputCh2.setPeriodHertz(490);    // standard 50 hz servo
    OutputCh2.attach(PIN_OUT_CH2, 1000, 2000);
}

void loop() {

    // handle controller input
    controller_handle([&](ControllerPtr controller) {

        if (controller->a()) {
            logi(LOG_TAG, "A");
        }

        // read left stick y axis
        int32_t ly = controller->axisY();
        if (ly > -DEADZONE_LY && ly < DEADZONE_LY) ly = 0;
        if (ly > BEEFZONE_LY)                      ly = 512;
        if (ly < -BEEFZONE_LY)                     ly = -512;
        // float ch1_out = ( (float)-ly / 4.0 ) + 128.0;
        ly += 512;
        int32_t ch1_out = map(ly, 0, 1024, speed_limit, 180-speed_limit);
        logd(LOG_TAG, "ly raw: %d, scaled: %d", ly, ch1_out);
        
        // write channel 1 output
        OutputCh1.write(ch1_out);


        // read left stick x axis
        int32_t lx = controller->axisX();
        if (lx > -DEADZONE_LX && lx < DEADZONE_LX) lx = 0;
        if (lx > BEEFZONE_LX)                      lx = 512;
        if (lx < -BEEFZONE_LX)                     lx = -512;
        lx += 512;
        int32_t ch2_out = map(1024 - lx, 0, 1024, speed_limit, 180-speed_limit);
        logd(LOG_TAG, "lx raw: %d, scaled: %d", lx, ch2_out);
        
        // write channel 1 output
        OutputCh2.write(ch2_out);


    });

}
