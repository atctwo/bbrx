
#include <vector>
#include <map>
#include <ESP32Servo.h>
#include "event_manager.h"
#include "controllers.h"
#include "log.h"
#include "config.h"

int16_t speed_limit = 60; // this is the amount by which the top speed (forwards + backwards) is reduced; when this is 0, max speed is 180

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
 * @brief Determine the current value of a given input event
 * 
 * @param event the event to get the value of
 * @param controller a pointer to the controller object to get the value of
 * @return int32_t 
 */
int32_t get_event_value(bb_event event, ControllerPtr controller) {

    switch(event) {
        case BB_EVENT_ANALOG_LX:            return controller->axisX();
        case BB_EVENT_ANALOG_LY:            return controller->axisY();
        case BB_EVENT_ANALOG_RX:            return controller->axisRX();
        case BB_EVENT_ANALOG_RY:            return controller->axisRY();
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

    int32_t out;

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
            servos[pin].write(out);

            break;

        case BB_ACTION_SPEED_UP:
            break;

        case BB_ACTION_SPEED_DOWN:
            break;

        case BB_ACTION_BREAK:
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
            int32_t event_value = get_event_value(bind.event, controller);

            // perform the action
            perform_action(bind.action, event_value, bind.min, bind.max, bind.pin);

        }

        // // read left stick y axis
        // int32_t ly = controller->axisY();
        // if (ly > -DEADZONE_LY && ly < DEADZONE_LY) ly = 0;
        // if (ly > BEEFZONE_LY)                      ly = 512;
        // if (ly < -BEEFZONE_LY)                     ly = -512;
        // // float ch1_out = ( (float)-ly / 4.0 ) + 128.0;
        // ly += 512;
        // int32_t ch1_out = map(ly, 0, 1024, speed_limit, 180-speed_limit);
        // logd(LOG_TAG, "ly raw: %d, scaled: %d", ly, ch1_out);
        
        // // write channel 1 output
        // OutputCh1.write(ch1_out);


        // // read left stick x axis
        // int32_t lx = controller->axisX();
        // if (lx > -DEADZONE_LX && lx < DEADZONE_LX) lx = 0;
        // if (lx > BEEFZONE_LX)                      lx = 512;
        // if (lx < -BEEFZONE_LX)                     lx = -512;
        // lx += 512;
        // int32_t ch2_out = map(1024 - lx, 0, 1024, speed_limit, 180-speed_limit);
        // logd(LOG_TAG, "lx raw: %d, scaled: %d", lx, ch2_out);
        
        // // write channel 1 output
        // OutputCh2.write(ch2_out);


    });

}

