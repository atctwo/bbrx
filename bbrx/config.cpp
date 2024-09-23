#include <stddef.h>
#include <memory>
#include <vector>
#include <Arduino.h>
#include <FS.h>
#include <FSImpl.h>
#include <LittleFS.h>
#include "sd_fat32_fs_wrapper.h"
#include "fkYAML/node.hpp"

#include "event_manager.h"
#include "bb_enums.h"
#include "log.h"
#include "config.h"

// include SdFat libraries
// since these are conditional, you can only use sdfat
// stuff if CONFIG_ENABLE_SD is defined!
#ifdef CONFIG_ENABLE_SD
    #include "SdFat.h"
    #include "sdios.h"
#endif

#define LOG_TAG "config"

int32_t DEADZONE_LY         = 32;      // inner deadzone for the left analog stick y axis
int32_t BEEFZONE_LY         = 500;     // outer deadzone for the left analog stick y axis
int32_t DEADZONE_LX         = 32;      // inner deadzone for the left analog stick x axis
int32_t BEEFZONE_LX         = 500;     // outer deadzone for the left analog stick x axis

int32_t DEADZONE_RY         = 32;      // inner deadzone for the right analog stick y axis
int32_t BEEFZONE_RY         = 500;     // outer deadzone for the right analog stick y axis
int32_t DEADZONE_RX         = 32;      // inner deadzone for the right analog stick x axis
int32_t BEEFZONE_RX         = 500;     // outer deadzone for the right analog stick x axis

int32_t DEADZONE_BRAKE      = 64;      // inner deadzone for the analog brake (L2)
int32_t BEEFZONE_BRAKE      = 1000;    // outer deadzone for the analog brake (L2)
int32_t DEADZONE_THROTTLE   = 64;      // inner deadzone for the analog throttle (R2)
int32_t BEEFZONE_THROTTLE   = 1000;    // outer deadzone for the analog throttle (R2)

/**
 * @brief A vector containing all currently registered bindings.
 * 
 * Consider the following when working with this vector:
 * - the initial contents of this vector _may_ be overwritten by the config loader
 * - before new bindings are used, they should be initialised by using event_manager:initialise_binding()
 */
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
 * @return true the document was parsed successfully and the config was loaded
 * @return false the document failed to parse or the config was not loaded
 */
bool parse_config(std::string yaml) {

    try {

        // deserialise the document to the root object
        fkyaml::node root = fkyaml::node::deserialize(yaml);

        // for each object to be parsed, this code checks that (1) the key to check if actually in the document, and
        // (2) the data type of the value matches what the code will expect it to be
        // (if the data type inferred from the yaml document does not match the type expected by this code then
        // this will break :(((   )
        // both these checks are done using the check_key() function

        // test string
        if (check_key(root, "test", fkyaml::node::node_t::STRING)) {
            std::string test = root["test"].get_value<std::string>();
            logd(LOG_TAG, "config test string: %s", test.c_str());
        }


        // get deadzones object
        if (check_key(root, "deadzones", fkyaml::node::node_t::MAPPING)) {

            logd(LOG_TAG, "loading deadzones...");
            auto &deadzones = root["deadzones"];

            if (check_key(deadzones, "ly", fkyaml::node::node_t::INTEGER)) {
                DEADZONE_LY = deadzones["ly"].get_value<int32_t>();
                logd(LOG_TAG, "- ly = %d", DEADZONE_LY);
            } else logd(LOG_TAG, "- couldn't get ly");

            if (check_key(deadzones, "lx", fkyaml::node::node_t::INTEGER)) {
                DEADZONE_LX = deadzones["lx"].get_value<int32_t>();
                logd(LOG_TAG, "- lx = %d", DEADZONE_LX);
            } else logd(LOG_TAG, "- couldn't get lx");

            if (check_key(deadzones, "ry", fkyaml::node::node_t::INTEGER)) {
                DEADZONE_RY = deadzones["ry"].get_value<int32_t>();
                logd(LOG_TAG, "- ry = %d", DEADZONE_RY);
            } else logd(LOG_TAG, "- couldn't get ry");

            if (check_key(deadzones, "rx", fkyaml::node::node_t::INTEGER)) {
                DEADZONE_RX = deadzones["rx"].get_value<int32_t>();
                logd(LOG_TAG, "- rx = %d", DEADZONE_RX);
            } else logd(LOG_TAG, "- couldn't get rx");

            if (check_key(deadzones, "brake", fkyaml::node::node_t::INTEGER)) {
                DEADZONE_BRAKE = deadzones["brake"].get_value<int32_t>();
                logd(LOG_TAG, "- brake = %d", DEADZONE_BRAKE);
            } else logd(LOG_TAG, "- couldn't get brake");

            if (check_key(deadzones, "throttle", fkyaml::node::node_t::INTEGER)) {
                DEADZONE_THROTTLE = deadzones["throttle"].get_value<int32_t>();
                logd(LOG_TAG, "- throttle = %d", DEADZONE_THROTTLE);
            } else logd(LOG_TAG, "- couldn't get throttle");

            // newline
            logd(LOG_TAG, "");

        } else logd(LOG_TAG, "failed to load deadzones");

        // get beefzones object
        if (check_key(root, "beefzones", fkyaml::node::node_t::MAPPING)) {

            logd(LOG_TAG, "loading beefzones...");
            auto &beefzones = root["beefzones"];

            if (check_key(beefzones, "ly", fkyaml::node::node_t::INTEGER)) {
                BEEFZONE_LY = beefzones["ly"].get_value<int32_t>();
                logd(LOG_TAG, "- ly = %d", BEEFZONE_LY);
            } else logd(LOG_TAG, "- couldn't get ly");

            if (check_key(beefzones, "lx", fkyaml::node::node_t::INTEGER)) {
                BEEFZONE_LX = beefzones["lx"].get_value<int32_t>();
                logd(LOG_TAG, "- lx = %d", BEEFZONE_LX);
            } else logd(LOG_TAG, "- couldn't get lx");

            if (check_key(beefzones, "ry", fkyaml::node::node_t::INTEGER)) {
                BEEFZONE_RY = beefzones["ry"].get_value<int32_t>();
                logd(LOG_TAG, "- ry = %d", BEEFZONE_RY);
            } else logd(LOG_TAG, "- couldn't get ry");

            if (check_key(beefzones, "rx", fkyaml::node::node_t::INTEGER)) {
                BEEFZONE_RX = beefzones["rx"].get_value<int32_t>();
                logd(LOG_TAG, "- rx = %d", BEEFZONE_RX);
            } else logd(LOG_TAG, "- couldn't get rx");

            if (check_key(beefzones, "brake", fkyaml::node::node_t::INTEGER)) {
                BEEFZONE_BRAKE = beefzones["brake"].get_value<int32_t>();
                logd(LOG_TAG, "- brake = %d", BEEFZONE_BRAKE);
            } else logd(LOG_TAG, "- couldn't get brake");

            if (check_key(beefzones, "throttle", fkyaml::node::node_t::INTEGER)) {
                BEEFZONE_THROTTLE = beefzones["throttle"].get_value<int32_t>();
                logd(LOG_TAG, "- throttle = %d", BEEFZONE_THROTTLE);
            } else logd(LOG_TAG, "- couldn't get throttle");

            // newline
            logd(LOG_TAG, "");

        } else logd(LOG_TAG, "failed to load beefzones");

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
                // check for conditionals key
                //------------------------
                // conditionals can either be a string or a list of strings, so check both

                // conditionals, string version
                if (check_key(bind, "conditionals", fkyaml::node::node_t::STRING)) {

                    // get conditionals as a string
                    std::string conditionals_str = bind["conditionals"].get_value<std::string>();
                    logd(LOG_TAG, "- conditional event string %s", conditionals_str.c_str());

                    // try to get enum id (int) from string
                    int conditionals_int = bb_event_to_enum(conditionals_str);
                    logd(LOG_TAG, "- conditional event int %d", conditionals_int);

                    // try to determine enum from int
                    if (conditionals_int == -1) {

                        logw(LOG_TAG, "invalid event for conditionals key in binding %d", i);
                        has_required = false;

                    } else {

                        bb_event conditional_event = (bb_event) conditionals_int;
                        logd(LOG_TAG, "- conditional enum %d", conditional_event);
                        bin.conditionals.push_back(conditional_event);

                    }

                } else {
                    logd(LOG_TAG, "- missing or invalid conditionals<string> key");
                }

                // conditionals, list of strings
                if (check_key(bind, "conditionals", fkyaml::node::node_t::SEQUENCE)) {

                    // for each item in sequence
                    for (int j = 0; j < bind["conditionals"].size(); j++) {

                        auto &conditional_item = bind["conditionals"][j];
                        logd(LOG_TAG, "- parsing conditional %d", j);

                        // get conditionals as a string
                        std::string conditionals_str = conditional_item.get_value<std::string>();
                        logd(LOG_TAG, "  - conditional event string %s", conditionals_str.c_str());

                        // try to get enum id (int) from string
                        int conditionals_int = bb_event_to_enum(conditionals_str);
                        logd(LOG_TAG, "  - conditional event int %d", conditionals_int);

                        // try to determine enum from int
                        if (conditionals_int == -1) {

                            logw(LOG_TAG, "invalid event (number %d) in conditionals list in binding %d", j, i);
                            has_required = false;

                        } else {

                            bb_event conditional_event = (bb_event) conditionals_int;
                            logd(LOG_TAG, "  - conditional enum %d", conditional_event);
                            bin.conditionals.push_back(conditional_event);

                        }
                    }

                } else {
                    logd(LOG_TAG, "- missing or invalid conditionals<sequence> key");
                }


                //------------------------
                // check for conditional_min key
                //------------------------

                if (check_key(bind, "conditional_min", fkyaml::node::node_t::INTEGER)) {

                    // get conditional_min as an int
                    int conditional_min = bind["conditional_min"].get_value<int>();
                    logd(LOG_TAG, "- conditional_min int %d", conditional_min);
                    bin.conditional_min = conditional_min;

                } else {
                    logd(LOG_TAG, "missing or invalid conditional_min key in binding %d", i);
                    bin.conditional_min = 0;
                }


                //------------------------
                // check for conditional_max key
                //------------------------

                if (check_key(bind, "conditional_max", fkyaml::node::node_t::INTEGER)) {

                    // get conditional_max as an int
                    int conditional_max = bind["conditional_max"].get_value<int>();
                    logd(LOG_TAG, "- conditional_max int %d", conditional_max);
                    bin.conditional_max = conditional_max;

                } else {
                    logd(LOG_TAG, "missing or invalid conditional_max key in binding %d", i);
                    bin.conditional_max = 1;
                }


                //------------------------
                // check for conditional_noexec key
                //------------------------

                if (check_key(bind, "conditional_noexec", fkyaml::node::node_t::BOOLEAN)) {

                    // get conditional_noexec as an bool
                    bool conditional_noexec = bind["conditional_noexec"].get_value<bool>();
                    logd(LOG_TAG, "- conditional_noexec bool %d", conditional_noexec);
                    bin.conditional_noexec = conditional_noexec;

                } else {
                    logd(LOG_TAG, "missing or invalid conditional_noexec key in binding %d", i);
                    bin.conditional_noexec = false;
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

        } else logw(LOG_TAG, "Failed to load bindings from %s", CONFIG_FILE_PATH);

        // once each config key has been checked, return true to indicate that the config was loaded ok
        return true;

    }
    catch (fkyaml::exception &e) {
        loge(LOG_TAG, "There was an error parsing %s:", CONFIG_FILE_PATH);
        loge(LOG_TAG, "%s", e.what());
    }

    // if it gets to this point then assume the loading has failed :(
    return false;

}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

bool open_config_file(fs::FS &fs) {

    File f = fs.open(CONFIG_FILE_PATH);
    if (!f) {
        logw(LOG_TAG, "Failed to open %s", CONFIG_FILE_PATH);
    } else {

        // read file into string
        std::string yaml;
        yaml.reserve(f.size());

        while (f.available()) {
            yaml += f.read();
        }

        // close file
        f.close();

        logv(LOG_TAG, "file read test:\n%s", yaml.c_str());

        // if the file was opened ok, try to parse it as yaml
        if (yaml != "") {
            bool res = parse_config(yaml);

            // if loading is successful, return from function
            return res;
        }

    }

    return false;

}

/**
 * @brief Load the bbrx config file from the configured filesystem(s)
 * @return true if the config was loaded from some file system
 * @return false if no config file was loaded, and defaults were kept
 */
bool load_config() {

    // attempt loading from card
    #ifdef CONFIG_ENABLE_SD

        logi(LOG_TAG, "Loading bbrx config file from SD...");

        // init SPI
        SPI.begin(CONFIG_SD_PIN_SCLK, CONFIG_SD_PIN_MISO, CONFIG_SD_PIN_MOSI);
        SPI.setFrequency(CONFIG_SD_SPI_FREQ);

        // try to init sd card
        SdFs sd;
        SdSpiConfig sd_config(CONFIG_SD_PIN_CS, USER_SPI_BEGIN, CONFIG_SD_SPI_FREQ);
        if (sd.begin(sd_config)) {

            // get fs:FS filesystem for use with open_config_file()
            // from https://github.com/greiman/SdFat/issues/471#issuecomment-2001960350
            fs::FS sd_fs = fs::FS(fs::FSImplPtr(new SdFat32FSImpl(sd)));

            // try to open and parse config file
            bool res = open_config_file(sd_fs);
            if (res) {
                logi(LOG_TAG, "Loaded config from SD card!");
                return true; 
            } // else continue with function

            // end sd
            sd.end();

        } else {
            LOG_OUTPUT.print("             "); // padding to line up with log prefix since i can't use my logging library i have to use serial!!!!!!!!! :(
            printSdErrorText(&LOG_OUTPUT, sd.card()->errorCode());
            LOG_OUTPUT.println("");
            logw(LOG_TAG, "Failed to initialise SD card (error code %d, data %d)", sd.card()->errorCode(), sd.card()->errorData());
        }

        // end spi
        SPI.end();

    #endif


    // attempt loading from littlefs
    #ifdef CONFIG_ENABLE_LITTLEFS

        logi(LOG_TAG, "Loading bbrx config file from LittleFS...");

        if (LittleFS.begin(CONFIG_LFS_FORMAT_IF_FAIL, CONFIG_LFS_BASE_PATH, CONFIG_LFS_MAX_OPEN_FILES, CONFIG_LFS_PARTITION_LABEL)) {

            // try to open and parse config file
            bool res = open_config_file(LittleFS);
            if (res) {
                logi(LOG_TAG, "Loaded config from LittleFS!");
                return true; 
            } // else continue with function

            // end littlefs
            LittleFS.end();

        } else logw(LOG_TAG, "Failed to initialise littlefs");

    #endif

    // if this point has been reached, then no attempt to load config.yml
    // has succeeded.  in this case, just use the default config values
    logi(LOG_TAG, "Could not load config file; using default config values");
    return false;

}