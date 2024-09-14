// #define RYML_SINGLE_HDR_DEFINE_NOW
// #include "ryml.hpp"

#include <Arduino.h>
#include <vector>

#include "fkYAML/node.hpp"

#include "event_manager.h"
#include "bb_enums.h"
#include "log.h"
#include "config.h"

#define LOG_TAG "config"

std::string test_yml = R"(
# bbrx config.yml test

test: 124

bindings:
# servo channel 1
- action: BB_ACTION_SERVO
  event: BB_EVENT_ANALOG_LY
  min: -512
  max: 511
  pin: 12

# servo channel 2
- action: BB_ACTION_SERVO
  event: BB_EVENT_ANALOG_LX
  min: 511
  max: -512
  pin: 13

# weapon control
- action: BB_ACTION_SERVO
  event: BB_EVENT_ANALOG_BRAKE
  min: -1024
  max: 1023
  pin: 14

- action: BB_ACTION_SERVO
  event: BB_EVENT_ANALOG_THROTTLE
  min: 1023
  max: -1024
  pin: 14

# speed control
- action: BB_ACTION_SPEED_UP
  event: BB_EVENT_BTN_B
  min: 0
  max: 1

- action: BB_ACTION_SPEED_UP
  event: BB_EVENT_DPAD_UP
  min: 0
  max: 1

- action: BB_ACTION_SPEED_DOWN
  event: BB_EVENT_BTN_X
  min: 0
  max: 1

- action: BB_ACTION_SPEED_DOWN
  event: BB_EVENT_DPAD_DOWN
  min: 0
  max: 1

# break
- action: BB_ACTION_BRAKE
  event: BB_EVENT_BTN_A
  min: 0
  max: 1

)";


std::vector<bb_binding> bindings = {

    /* movement (servo ch1 + ch2) */
    {.action = BB_ACTION_SERVO, .event = BB_EVENT_ANALOG_LY, .min = -512, .max =  511, .pin = 12},
    {.action = BB_ACTION_SERVO, .event = BB_EVENT_ANALOG_LX, .min =  511, .max = -512, .pin = 13},

    /* weapon control */
    {.action = BB_ACTION_SERVO, .event = BB_EVENT_ANALOG_BRAKE,    .min =  -1024, .max =  1023, .pin = 14},
    {.action = BB_ACTION_SERVO, .event = BB_EVENT_ANALOG_THROTTLE, .min =   1023, .max = -1024, .pin = 14},

    /* brake */
    {.action = BB_ACTION_BRAKE, .event = BB_EVENT_BTN_A, .min = 0, .max = 1},

    /* speed control */
    {.action = BB_ACTION_SPEED_UP, .event = BB_EVENT_BTN_B, .min = 0, .max = 1},
    {.action = BB_ACTION_SPEED_DOWN, .event = BB_EVENT_BTN_X, .min = 0, .max = 1},
    {.action = BB_ACTION_SPEED_UP, .event = BB_EVENT_DPAD_UP, .min = 0, .max = 1},
    {.action = BB_ACTION_SPEED_DOWN, .event = BB_EVENT_DPAD_DOWN, .min = 0, .max = 1},
};

/**
 * Returns true if the specified node has a key with the name key, that is of the specified type
 */
bool check_key(fkyaml::node &node, const char *key, fkyaml::node::node_t val_type) {
    return (node.contains(key)) && (node[key].type() == val_type);
}

/**
 * @brief Parse a YAML document for bbrx config stuff
 * 
 * This function parses a YAML document for bbrx config data.  If the document is parsed successfully, the 
 * existing `bindings` vector is cleared and replaced with whatever bindings are successfully parsed from
 * the YAML document.
 * 
 * Existing bindings are cleared if the document and the bindings object are parsed successfully (even if
 * there are no actual valid bindings).  Individual bindings can fail to be registered if they do not use
 * valid YAML syntax, or are missing any of the required keys
 * 
 * If the document fails to parse, then no changes are made to any config variable, and the previous
 * bindset remains unchanged.
 * 
 * Please see the usage docs for info on the contents of the YAML document!
 * 
 * @param yaml the document to parse
 */
void parse_config(std::string yaml) {

    // deserialise the document to the root object
    fkyaml::node root = fkyaml::node::deserialize(yaml);

    // for each object to be parsed, this code checks that (1) the key to check if actually in the document, and
    // (2) the data type of the value matches what the code will expect it to be
    // (if the data type inferred from the yaml document does not match the type expected by this code then
    // this will break :(((   )
    // both these checks are done using the check_key() function

    // get bindings object
    if (check_key(root, "bindings", fkyaml::node::node_t::SEQUENCE)) {

        // first, clear existing bindings
        bindings.clear();

        // for each binding
        for (int i = 0; i < root["bindings"].size(); i++) {
            auto &bind = root["bindings"][i];

            logd(LOG_TAG, "parsing binding %d", i);

            // flag which will be set to false if any required parameters are missing
            bool has_required = true;

            // create a binding struct to populate
            bb_binding bin;

            //------------------------
            // check for action key
            //------------------------

            if (check_key(bind, "action", fkyaml::node::node_t::STRING)) {

                // get action as a string
                std::string action_str = bind["action"].get_value<std::string>();
                logd(LOG_TAG, "- action string %s", action_str.c_str());

                // try to get enum id (int) from string
                int action_int = bb_action_to_enum(action_str);
                logd(LOG_TAG, "- action int %d", action_int);

                // try to determine enum from int
                if (action_int == -1) {

                    logw(LOG_TAG, "invalid action key in binding %d", i);
                    has_required = false;

                } else {

                    bb_action action = (bb_action) action_int;
                    logd(LOG_TAG, "- action enum %d", action);
                    bin.action = action;

                }

            } else {
                logw(LOG_TAG, "missing action key in binding %d", i);
                has_required = false;
            }


            //------------------------
            // check for event key
            //------------------------

            if (check_key(bind, "event", fkyaml::node::node_t::STRING)) {
                
                // get event as a string
                std::string event_str = bind["event"].get_value<std::string>();
                logd(LOG_TAG, "- event string %s", event_str.c_str());

                // try to get enum id (int) from string
                int event_int = bb_event_to_enum(event_str);
                logd(LOG_TAG, "- event int %d", event_int);

                // try to determine enum from int
                if (event_int == -1) {

                    logw(LOG_TAG, "invalid event key in binding %d", i);
                    has_required = false;

                } else {

                    bb_event event = (bb_event) event_int;
                    logd(LOG_TAG, "- event enum %d", event);
                    bin.event = event;

                }

            } else {
                logw(LOG_TAG, "missing event key in binding %d", i);
                has_required = false;
            }


            //------------------------
            // check for min key
            //------------------------

            if (check_key(bind, "min", fkyaml::node::node_t::INTEGER)) {

                // get min as an int
                int min = bind["min"].get_value<int>();
                logd(LOG_TAG, "- min int %d", min);
                bin.min = min;

            } else {
                logw(LOG_TAG, "missing or invalid min key in binding %d", i);
                has_required = false;
            }


            //------------------------
            // check for max key
            //------------------------

            if (check_key(bind, "max", fkyaml::node::node_t::INTEGER)) {

                // get max as an int
                int max = bind["max"].get_value<int>();
                logd(LOG_TAG, "- max int %d", max);
                bin.max = max;

            } else {
                logw(LOG_TAG, "missing or invalid max key in binding %d", i);
                has_required = false;
            }


            //------------------------
            // check for default_value key
            //------------------------

            if (check_key(bind, "default_value", fkyaml::node::node_t::INTEGER)) {

                // get default_value as an int
                int default_value = bind["default_value"].get_value<int>();
                logd(LOG_TAG, "- default_value int %d", default_value);
                bin.default_value = default_value;

            } else {
                logd(LOG_TAG, "- missing or invalid default_value key");
                bin.default_value = 0;
            }


            //------------------------
            // check for pin key
            //------------------------

            if (check_key(bind, "pin", fkyaml::node::node_t::INTEGER)) {

                // get pin as an int
                int pin = bind["pin"].get_value<int>();
                logd(LOG_TAG, "- pin int %d", pin);
                bin.pin = pin;

            } else {
                logd(LOG_TAG, "- missing or invalid pin key");
                bin.pin = 0;
            }


            //------------------------
            // check for exec_without_controller key
            //------------------------

            if (check_key(bind, "exec_without_controller", fkyaml::node::node_t::BOOLEAN)) {

                // get exec_without_controller as a bool
                bool exec_without_controller = bind["exec_without_controller"].get_value<int>();
                logd(LOG_TAG, "- exec_without_controller int %d", exec_without_controller);
                bin.exec_without_controller = exec_without_controller;

            } else {
                logd(LOG_TAG, "- missing or invalid exec_without_controller key");
                bin.exec_without_controller = false;
            }


            //------------------------
            // check for ignore_claims key
            //------------------------

            if (check_key(bind, "ignore_claims", fkyaml::node::node_t::BOOLEAN)) {

                // get ignore_claims as a bool
                bool ignore_claims = bind["ignore_claims"].get_value<int>();
                logd(LOG_TAG, "- ignore_claims int %d", ignore_claims);
                bin.ignore_claims = ignore_claims;

            } else {
                logd(LOG_TAG, "- missing or invalid ignore_claims key");
                bin.ignore_claims = false;
            }


            //------------------------
            // after all params have been parsed, add to bindings (if all required params are passed)
            //------------------------

            if (has_required) {
                // add to bindings vector
                logd(LOG_TAG, "- adding binding");
                bindings.push_back(bin);
            }

            // print newline
            logd(LOG_TAG, "");
        }

    } else logw(LOG_TAG, "failed to load bindings object from config.yml");

}

/**
 * @brief Load the bbrx config file from the configured filesystem(s)
 */
void load_config() {

    // attempt loading from card
    #ifdef CONFIG_ENABLE_SD

        logi(LOG_TAG, "Loading bbrx config file from SD...");

    #endif


    // attempt loading from littlefs
    #ifdef CONFIG_ENABLE_LITTLEFS

        logi(LOG_TAG, "Loading bbrx config file from LittleFS...");

    #endif

    // if this point has been reached, then no attempt to load config.yml
    // has succeeded.  in this case, just use the default config values
    logi(LOG_TAG, "Could not load config file; using default config values");

    parse_config(test_yml);

}