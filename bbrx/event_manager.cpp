
#include <vector>
#include <map>
#include <ESP32Servo.h>
#include "event_manager.h"
#include "controllers.h"
#include "log.h"
#include "config.h"

int16_t speed_limit = 60; // this is the amount by which the top speed (forwards + backwards) is reduced; when this is 0, max speed is 180
bool brake = false;

#define LOG_TAG "events"

/**
 * @brief Struct to hold details for each binding
 * 
 */
struct binding {
    bb_action action;
    bb_event  event;
    int32_t   min;
    int32_t   max;
    uint8_t   pin;
    // some variable for analog input -> digital action, which specifies
    // positive or negative (ie: resolve as true if the value is 
    // greater than (pos) or less than (neg) the range midpoint).
    // probably doesn't matter for analog actions
};

/**
 * @brief Vector to store each registered binding
 * 
 */
std::vector<binding> bindings;

/**
 * @brief Vector to hold Servo objects for each pin which is used for servo output
 * 
 */
std::map<uint8_t, Servo> servos;

/**
 * @brief Register an event binding
 * 
 * @param action the receiver action to perform when the event occurs
 * @param event the event to which the action should respond
 * @param min minimum value of the range of possible inputs
 * @param max maximum value of the range of possible inputs
 * @param pin which pin to use as output (optional)
 */
void register_binding(bb_action action, bb_event event, int32_t min, int32_t max, uint8_t pin) {
    
    // if a pin is registered for a servo action, create a servo object for that pin
    Servo *servo = new Servo();
    servo->setPeriodHertz(ESC_PWM_FREQ);
    servo->attach(pin, ESC_PWM_MIN, ESC_PWM_MAX);
    servos[pin] = *servo;

    // register binding
    binding b = {action, event, min, max, pin};
    bindings.push_back(b);
}

/**
 * @brief Apply a deadzone to a controller input
 * 
 * @param input the current value of the input
 * @param dead the minimum input value to not be considered 0 (both + and -)
 * @param beef the maximum input value to not be considered max (both + and -)
 * @param min the minimum value of the range of inputs
 * @param max the maximum value of the range of inputs
 * @return int32_t 
 */
int32_t deadzone(int32_t input, int32_t dead, int32_t beef, int32_t min, int32_t max) {
    if (input > -dead && input < dead) input = 0;
    if (input > beef)                  input = max;
    if (input < -beef)                 input = -max;
    // float ch1_out = ( (float)-input / 4.0 ) + 128.0;
    // input += max;

    return input;
}

/**
 * @brief Determine the current value of a given input event
 * 
 * @param event the event to get the value of
 * @param controller a pointer to the controller object to get the value of
 * @return int32_t 
 */
int32_t get_event_value(bb_event event, ControllerPtr controller, int32_t min, int32_t max) {

    switch(event) {
        case BB_EVENT_ANALOG_LX:            return deadzone(controller->axisX(), DEADZONE_LX, BEEFZONE_LX, min, max);
        case BB_EVENT_ANALOG_LY:            return deadzone(controller->axisY(), DEADZONE_LY, BEEFZONE_LY, min, max);
        case BB_EVENT_ANALOG_RX:            return deadzone(controller->axisRX(), DEADZONE_LX, BEEFZONE_LX, min, max);
        case BB_EVENT_ANALOG_RY:            return deadzone(controller->axisRY(), DEADZONE_LY, BEEFZONE_LY, min, max);
        case BB_EVENT_ANALOG_BRAKE:         return controller->brake();
        case BB_EVENT_ANALOG_THROTTLE:      return controller->throttle();
        case BB_EVENT_GYRO_X:               return controller->gyroX();
        case BB_EVENT_GYRO_Y:               return controller->gyroY();
        case BB_EVENT_GYRO_Z:               return controller->gyroY();
        case BB_EVENT_ACCEL_X:              return controller->accelX();
        case BB_EVENT_ACCEL_Y:              return controller->accelY();
        case BB_EVENT_ACCEL_Z:              return controller->accelZ();
        case BB_EVENT_BTN_A:                return controller->a();
        case BB_EVENT_BTN_B:                return controller->b();
        case BB_EVENT_BTN_X:                return controller->x();
        case BB_EVENT_BTN_Y:                return controller->y();
        case BB_EVENT_BTN_L1:               return controller->l1();
        case BB_EVENT_BTN_L2:               return controller->l2();
        case BB_EVENT_BTN_R1:               return controller->r1();
        case BB_EVENT_BTN_R2:               return controller->r2();
        case BB_EVENT_BTN_L3:               return controller->thumbL();
        case BB_EVENT_BTN_R3:               return controller->thumbR();
        case BB_EVENT_BTN_SYSTEM:           return controller->miscSystem();
        case BB_EVENT_BTN_START:            return controller->miscStart();
        case BB_EVENT_BTN_SELECT:           return controller->miscSelect();
        case BB_EVENT_BTN_CAPTURE:          return controller->miscCapture();
        case BB_EVENT_MOUSE_DX:             return controller->deltaX();
        case BB_EVENT_MOUSE_DY:             return controller->deltaY();
        case BB_EVENT_MOUSE_SCROLLWHEEL:    return controller->scrollWheel();
        case BB_EVENT_WII_BB_TOP_LEFT:      return controller->topLeft();
        case BB_EVENT_WII_BB_TOP_RIGHT:     return controller->topRight();
        case BB_EVENT_WII_BB_BOTTOM_LEFT:   return controller->bottomLeft();
        case BB_EVENT_WII_BB_BOTTOM_RIGHT:  return controller->bottomRight();
        case BB_EVENT_WII_BB_TEMPERATURE:   return controller->temperature();
        case BB_EVENT_MISC_BATTERY:         return controller->battery();

        default:
            logw(LOG_TAG, "Unknown event value requested (event=%d)", event);
            return 0;
    }

}

/**
 * @brief Perform the specified action
 * 
 * @param action the receiver action to perform when the event occurs
 * @param event_value the current value of the event to which the action should respond
 * @param min minimum value of the range of possible inputs
 * @param max maximum value of the range of possible inputs
 * @param pin which pin to use as output (optional)
 */
void perform_action(bb_action action, int32_t event_value, int32_t min, int32_t max, uint8_t pin) {

    int32_t out, input;

    switch(action) {
        case BB_ACTION_TEST:

            // map input to boolean
            // logi(LOG_TAG, "value=%d\tout=%d", event_value, map(event_value, min, max, 0, 1));
            if (event_value > ((max - min) / 2) + min) {
                logi(LOG_TAG, "event manager test!!! pin=%d", pin);
            }

            break;

        case BB_ACTION_SERVO:

            out = map(event_value, min, max, ESC_PWM_MIN, ESC_PWM_MAX);
            logd(LOG_TAG, "servo out: raw: %d, scaled: %d", event_value, out);
            
            // write channel output
            if (brake) servos[pin].writeMicroseconds(ESC_PWM_MID);
            else       servos[pin].writeMicroseconds(out);


            break;

        case BB_ACTION_SPEED_UP:
            break;

        case BB_ACTION_SPEED_DOWN:
            break;

        case BB_ACTION_BREAK:

            input = (event_value > ((max - min) / 2) + min);
            if (!brake && input) logi(LOG_TAG, "Breaking!");
            if (brake && !input) logi(LOG_TAG, "Stepping off the breaks...");

            brake = input;

            break;

        default:
            logw(LOG_TAG, "Tried to call unsupported action (action=%d)", action);
    }

}

/**
 * @brief Set up the event manager and all the hardware required for each implemented action
 * 
 */
void event_manager_setup() {

    // setup servo pwm
    ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);

}

/**
 * @brief Check controller input and perform bound actions
 * 
 */
void event_manager_update() {

    // handle controller input
    controller_handle([&](ControllerPtr controller) {

        // for each binding
        for (binding bind : bindings) {

            // determine the event value from the event type
            int32_t event_value = get_event_value(bind.event, controller, bind.min, bind.max);

            // perform the action
            perform_action(bind.action, event_value, bind.min, bind.max, bind.pin);

        }

    });

}

