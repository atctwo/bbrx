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
    std::vector<bb_event> conditionals;             // array of events which must evaluate as true before the action can be called
    int32_t   conditional_min;                      // minimum value of the range of inputs that the conditional event(s) could have
    int32_t   conditional_max;                      // maximum value of the range of inputs that the conditional event(s) could have
    bool      conditional_noexec;                   // if true, don't run the action when conditionals fail (otherwise do run with default value)
};
// note: if adding members to this struct make sure to add code in config.cpp:parse_config() to interpret the param from the config yaml file

void initialise_binding(bb_binding b);
void event_manager_setup();
void event_manager_update();