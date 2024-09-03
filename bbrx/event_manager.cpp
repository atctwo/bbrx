
#include <vector>
#include <map>
#include <ESP32Servo.h>
#include "event_manager.h"
#include "controllers.h"
#include "log.h"
#include "config.h"

int16_t speed_limit = 0;  // this is the amount by which the top speed (forwards + backwards) is reduced; when this is 0, max speed is 180
bool brake = false;       // if true, then all servos will stop

#define LOG_TAG "events"

/**
 * @brief Vector to store each registered binding
 * 
 */
std::vector<bb_binding> bindings;

/**
 * @brief Map to hold Servo objects for each pin which is used for servo output
 * 
 */
std::map<uint8_t, Servo> servos;

/**
 * @brief Map to keep track of whether each action is claimed by which binding
 * 
 * The value for this map is a pair, where the first element is a boolean indicating whether
 * the action has been claimed, and the second element is the ID of the binding
 * which has claimed it.
 * 
 * The structure of this map is as follows:
 * 
 * action_claims (map)
 * ├── key: action (bb_action)
 * └── value: (map) of each action to each pin
 *     ├── key: pin number (uint8_t)
 *     └── value: (pair) with claim info
 *         ├── first: whether there is a claim for this action on this pin (bool)
 *         └── second: id of the binding which has this claim (uint16_t)
 * 
 * This structure stores claim info pairs under two keys: action and pin number.
 * This means that each claim is specific to each combination of action and 
 * pin number.
 * 
 * To access the claim info for a specific action and pin, use the following
 * approach:
 * 
 * ```
 * action_claims[action][pin].first  // returns whether there's a claim
 * action_claims[action][pin].second // returns which binding has the claim
 * ```
 */
std::map<bb_action, std::map<uint8_t, std::pair<bool, uint16_t>>> action_claims;

/**
 * @brief Register an event binding
 * 
 * @param b a bbrx_binding object which contains all the details of the binding
 */
void register_binding(bb_binding b) {
    
    // if a pin is registered for a servo action, create a servo object for that pin
    if (b.action == BB_ACTION_SERVO) {
        Servo *servo = new Servo();
        servo->setPeriodHertz(ESC_PWM_FREQ);
        servo->attach(b.pin, ESC_PWM_MIN, ESC_PWM_MAX);
        servos[b.pin] = *servo;
    }

    // register binding
    bindings.push_back(b);

    logi(LOG_TAG, "Registered binding %d: action=%d, event=%d", bindings.size()-1, b.action, b.event);
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
int32_t deadzone(int32_t input, int32_t dead, int32_t beef, int32_t min_value, int32_t max_value) {
    if (input > -dead && input < dead) input = 0;
    if (input > beef)                  input = max(min_value, max_value);
    if (input < -beef)                 input = min(min_value, max_value);
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
        case BB_EVENT_ANALOG_LX:            return deadzone(controller->axisX(),    DEADZONE_LX,       BEEFZONE_LX,       min, max);
        case BB_EVENT_ANALOG_LY:            return deadzone(controller->axisY(),    DEADZONE_LY,       BEEFZONE_LY,       min, max);
        case BB_EVENT_ANALOG_RX:            return deadzone(controller->axisRX(),   DEADZONE_LX,       BEEFZONE_LX,       min, max);
        case BB_EVENT_ANALOG_RY:            return deadzone(controller->axisRY(),   DEADZONE_LY,       BEEFZONE_LY,       min, max);
        case BB_EVENT_ANALOG_BRAKE:         return deadzone(controller->brake(),    DEADZONE_BRAKE,    BEEFZONE_BRAKE,    min, max);
        case BB_EVENT_ANALOG_THROTTLE:      return deadzone(controller->throttle(), DEADZONE_THROTTLE, BEEFZONE_THROTTLE, min, max);
        case BB_EVENT_GYRO_X:               return controller->gyroX();
        case BB_EVENT_GYRO_Y:               return controller->gyroY();
        case BB_EVENT_GYRO_Z:               return controller->gyroY();
        case BB_EVENT_ACCEL_X:              return controller->accelX();
        case BB_EVENT_ACCEL_Y:              return controller->accelY();
        case BB_EVENT_ACCEL_Z:              return controller->accelZ();
        case BB_EVENT_DPAD_UP:              return controller->dpad() & 0x01;
        case BB_EVENT_DPAD_DOWN:            return controller->dpad() & 0x02;
        case BB_EVENT_DPAD_LEFT:            return controller->dpad() & 0x08;
        case BB_EVENT_DPAD_RIGHT:           return controller->dpad() & 0x04;
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
void perform_action(int32_t event_value, bb_binding bind, ControllerPtr controller=nullptr) {

    int32_t out, input;

    switch(bind.action) {
        case BB_ACTION_TEST:

            // map input to boolean
            // logi(LOG_TAG, "value=%d\tout=%d", event_value, map(event_value, min, max, 0, 1));
            if (event_value > ((bind.max - bind.min) / 2) + bind.min) {
                logi(LOG_TAG, "event manager test!!! pin=%d", bind.pin);
            }

            break;

        case BB_ACTION_SERVO:

            out = map(event_value, bind.min, bind.max, ESC_PWM_MIN + speed_limit, ESC_PWM_MAX - speed_limit);
            logd(LOG_TAG, "servo out: raw: %d, scaled: %d", event_value, out);
            
            // write channel output
            if (brake) servos[bind.pin].writeMicroseconds(ESC_PWM_MID);
            else       servos[bind.pin].writeMicroseconds(out);


            break;

        case BB_ACTION_SPEED_UP:
            if (event_value > ((bind.max - bind.min) / 2) + bind.min) {
                speed_limit--;
                if (speed_limit < 0) speed_limit = 0;
                logi(LOG_TAG, "Decreasing speed restriction to %d", speed_limit);
            }
            break;

        case BB_ACTION_SPEED_DOWN:
            if (event_value > ((bind.max - bind.min) / 2) + bind.min) {
                speed_limit++;
                if (speed_limit > (ESC_PWM_MAX-ESC_PWM_MIN)/2) speed_limit = (ESC_PWM_MAX-ESC_PWM_MIN)/2;
                logi(LOG_TAG, "Increasing speed restriction to %d", speed_limit);
            }
            break;

        case BB_ACTION_SPEED_SET:

            out = map(event_value, bind.min, bind.max, 0, (ESC_PWM_MAX-ESC_PWM_MIN)/2);
            logi(LOG_TAG, "speed set: raw: %d, scaled: %d", event_value, out);
            speed_limit = out;
            break;

        case BB_ACTION_BREAK:

            input = (event_value > ((bind.max - bind.min) / 2) + bind.min);
            if (!brake && input) logi(LOG_TAG, "Breaking!");
            if (brake && !input) logi(LOG_TAG, "Stepping off the breaks...");

            brake = input;

            break;

        default:
            logw(LOG_TAG, "Tried to call unsupported action (action=%d)", bind.action);
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
        for (uint16_t bind_id = 0; bind_id < bindings.size(); bind_id++) {

            // get reference to binding object
            bb_binding &bind = bindings[bind_id];

            // first check if the action hasn't yet already been claimed by another binding
            // or if the action is claimed by this binding
            // (or if the binding ignores claims just resolve as true)
            if ((!action_claims[bind.action][bind.pin].first) || 
                ( action_claims[bind.action][bind.pin].second == bind_id) || 
                bind.ignore_claims
            ) {

                // variable for storing value of the bound event
                int32_t event_value = bind.default_value;

                // if controller is connected
                if (controller != nullptr) {

                    // determine the event value from the event type
                    event_value = get_event_value(bind.event, controller, bind.min, bind.max);

                }
                // otherwise assume the default

                // set claim flag if not already claimed
                // but only if the input is non-default
                if (event_value != bind.default_value) {
                    if (!action_claims[bind.action][bind.pin].first) {
                        action_claims[bind.action][bind.pin].first = true;
                        action_claims[bind.action][bind.pin].second = bind_id;
                        // logi(LOG_TAG, "Action %d on pin %d claimed by binding %d", bind.action, bind.pin, bind_id);
                    }
                }
                // if the input _is_ the default, assume the action has been unclaimed
                // (but only if this is the claimant binding)
                else if (action_claims[bind.action][bind.pin].second == bind_id) {
                    if (action_claims[bind.action][bind.pin].first) {
                        action_claims[bind.action][bind.pin].first = false;
                        action_claims[bind.action][bind.pin].second = 0;
                        // logi(LOG_TAG, "Action %d on pin %d unclaimed by binding %d", bind.action, bind.pin, bind_id);
                    }
                }

                // perform the action if controller is connected, or exec without controller is enabled for this binding
                if ((controller != nullptr) || bind.exec_without_controller) {
                    // logi(LOG_TAG, "acting value=%d", event_value);
                    // Serial.printf("%d\t", event_value);
                    perform_action(event_value, bind, controller);
                }

            }

        }

        // Serial.println("");

    });

    // if no controllers are connected
    if (!controller_connected()) {

        // Failsafe: kill motors when nothing is connected
        #if defined(ENABLE_FAILSAFES) and defined(FAILSAFE_NO_CONTROLLER)

            // for each servo {don't}
            for (auto it = servos.begin(); it != servos.end(); ++it) {

                // write midpoint value to each servo motor (ie: turn it off)
                it->second.writeMicroseconds(ESC_PWM_MID);
            }

        #endif

    }

}