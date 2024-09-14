#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <algorithm>

#include "bb_enums.h"

/**
 * @brief Struct to hold details for each binding
 * 
 */
struct bb_binding {
    bb_action action;                               // the receiver action to perform when the event occurs
    bb_event  event;                                // the event to which the action should respond
    int32_t   min;                                  // minimum value of the range of possible inputs
    int32_t   max;                                  // maximum value of the range of possible inputs
    int32_t   default_value;                        // the default / neural position value for the event (for when no controller is connected and detecting when inputs are neutral)
    uint8_t   pin;                                  // which pin to use as output
    bool      exec_without_controller;              // whether to execute the action if a controller isn't connected (with event value = 0)
    bool      ignore_claims;                        // if true, execute the bound action even if it is claimed by another binding
};
// note: if adding members to this struct make sure to add code in config.cpp:parse_config() to interpret the param from the config yaml file

void initialise_binding(bb_binding b);
void event_manager_setup();
void event_manager_update();