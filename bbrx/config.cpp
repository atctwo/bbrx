// #define RYML_SINGLE_HDR_DEFINE_NOW
// #include "ryml.hpp"

#include <Arduino.h>
#include <vector>

#include "fkYAML/node.hpp"

#include "event_manager.h"
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

- action: BB_EVENT_SPEED_UP
  event: BB_EVENT_DPAD_UP
  min: 0
  max: 1

- action: BB_EVENT_SPEED_DOWN
  event: BB_EVENT_BTN_X
  min: 0
  max: 1

- action: BB_EVENT_SPEED_DOWN
  event: BB_EVENT_DPAD_DOWN
  min: 0
  max: 1

# break
- action: BB_ACTION_BRAKE
  event: BB_EVENT_BTN_A
  min: 0
  max: 1

)";


std::vector<bb_binding> bindings = {                                                                                   \
                                                                                                                \
    /* movement (servo ch1 + ch2) */                                                                            \
    {.action = BB_ACTION_SERVO, .event = BB_EVENT_ANALOG_LY, .min = -512, .max =  511, .pin = 12},              \
    {.action = BB_ACTION_SERVO, .event = BB_EVENT_ANALOG_LX, .min =  511, .max = -512, .pin = 13},              \
                                                                                                                \
    /* weapon control */                                                                                        \
    {.action = BB_ACTION_SERVO, .event = BB_EVENT_ANALOG_BRAKE,    .min =  -1024, .max =  1023, .pin = 14},     \
    {.action = BB_ACTION_SERVO, .event = BB_EVENT_ANALOG_THROTTLE, .min =   1023, .max = -1024, .pin = 14},     \
                                                                                                                \
    /* brake */                                                                                                 \
    {.action = BB_ACTION_BRAKE, .event = BB_EVENT_BTN_A, .min = 0, .max = 1},                                   \
                                                                                                                \
    /* speed control */                                                                                         \
    {.action = BB_ACTION_SPEED_UP, .event = BB_EVENT_BTN_B, .min = 0, .max = 1},                                \
    {.action = BB_ACTION_SPEED_DOWN, .event = BB_EVENT_BTN_X, .min = 0, .max = 1},                              \
    {.action = BB_ACTION_SPEED_UP, .event = BB_EVENT_DPAD_UP, .min = 0, .max = 1},                              \
    {.action = BB_ACTION_SPEED_DOWN, .event = BB_EVENT_DPAD_DOWN, .min = 0, .max = 1},                          \
};

/**
 * Returns true if the specified node has a key with the name key, that is of the specified
 * type
 */
bool check_key(fkyaml::node &node, const char *key, fkyaml::node::node_t val_type) {

    return (node.contains(key)) && (node[key].type() == val_type);

}

void parse_config() {

    // ryml::Tree tree = ryml::parse_in_arena(test_yml);
    // ryml::ConstNodeRef root = tree.crootref();

    // // emit to a buffer:
    // // std::string str_result = ryml::emitrs_yaml<std::string>(tree);
    // // logi(LOG_TAG, "raw yaml: \n%s", test_yml);
    // // logi(LOG_TAG, "parsed yaml: \n%s", str_result.c_str());

    // logi(LOG_TAG, "a");
    // ryml::NodeRef test_node = root["test2"];
    // logi(LOG_TAG, "b");
    // if (root["test2"].has_val()) {
    //     logi(LOG_TAG, "c");

    //     logi(LOG_TAG, "hello :)");

    //     int t2 = 0;
    //     root["test2"] >> t2;
    //     logi(LOG_TAG, "yaml test 2: %d", t2);

    // } else {
    //     logw(LOG_TAG, "test not in yml");
    // }

    // logi(LOG_TAG, "a");
    // logi(LOG_TAG, "b");

    // switch(root["test"].type()) {
    //     case fkyaml::node::node_t::STRING  : logi(LOG_TAG, "string"); break;
    //     case fkyaml::node::node_t::INTEGER  : logi(LOG_TAG, "int"); break;
    //     case fkyaml::node::node_t::FLOAT_NUMBER  : logi(LOG_TAG, "float"); break;
    //     case fkyaml::node::node_t::BOOLEAN  : logi(LOG_TAG, "bool"); break;
    //     case fkyaml::node::node_t::NULL_OBJECT  : logi(LOG_TAG, "null"); break;
    //     case fkyaml::node::node_t::MAPPING  : logi(LOG_TAG, "mapping"); break;
    //     case fkyaml::node::node_t::SEQUENCE  : logi(LOG_TAG, "sequence"); break;
    // }

    // int AAA = root["test"].get_value<int>();
    // logi(LOG_TAG, "c");


    // logi(LOG_TAG, "aaaaaaaaaa %d", AAA);
    // logi(LOG_TAG, "d");

    fkyaml::node root = fkyaml::node::deserialize(test_yml);


    if (check_key(root, "test", fkyaml::node::node_t::INTEGER)) {
        int v = root["test"].get_value<int>();
        logi(LOG_TAG, "test 1: %d :)", v);
    } else logi(LOG_TAG, "test 1 failed :(");

    if (check_key(root, "test", fkyaml::node::node_t::STRING)) {
        int v = root["test"].get_value<int>();
        logi(LOG_TAG, "test 2: %d :(", v);
    } else logi(LOG_TAG, "test 2 failed :)");

    if (check_key(root, "test2", fkyaml::node::node_t::INTEGER)) {
        int v = root["test2"].get_value<int>();
        logi(LOG_TAG, "test 2: %d :(", v);
    } else logi(LOG_TAG, "test 2 failed :)");

    if (check_key(root, "test2", fkyaml::node::node_t::STRING)) {
        int v = root["test2"].get_value<int>();
        logi(LOG_TAG, "test 2: %d :(", v);
    } else logi(LOG_TAG, "test 2 failed :)");
}

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

    parse_config();

}