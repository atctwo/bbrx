#pragma once

#include <cstdint>

/**
 * @brief Enum containing supported receiver actions
 * 
 */
enum bb_action {
    BB_ACTION_TEST,
    BB_ACTION_SERVO,
    BB_ACTION_SPEED_UP,
    BB_ACTION_SPEED_DOWN,
    BB_ACTION_BREAK
};

/**
 * @brief Enum containing supported gamepad events
 * 
 */
enum bb_event {

    // controller analog inputs
    BB_EVENT_ANALOG_LX,
    BB_EVENT_ANALOG_LY,
    BB_EVENT_ANALOG_RX,
    BB_EVENT_ANALOG_RY,
    BB_EVENT_ANALOG_BRAKE,
    BB_EVENT_ANALOG_THROTTLE,

    // controller motion controls
    BB_EVENT_GYRO_X,
    BB_EVENT_GYRO_Y,
    BB_EVENT_GYRO_Z,
    BB_EVENT_ACCEL_X,
    BB_EVENT_ACCEL_Y,
    BB_EVENT_ACCEL_Z,

    // controller buttons
    BB_EVENT_BTN_A,
    BB_EVENT_BTN_B,
    BB_EVENT_BTN_X,
    BB_EVENT_BTN_Y,
    BB_EVENT_BTN_L1,
    BB_EVENT_BTN_L2,
    BB_EVENT_BTN_R1,
    BB_EVENT_BTN_R2,
    BB_EVENT_BTN_L3,
    BB_EVENT_BTN_R3,
    BB_EVENT_BTN_SYSTEM,
    BB_EVENT_BTN_START,
    BB_EVENT_BTN_SELECT,
    BB_EVENT_BTN_CAPTURE,

    // mouse events
    BB_EVENT_MOUSE_DX,
    BB_EVENT_MOUSE_DY,
    BB_EVENT_MOUSE_SCROLLWHEEL,

    // wii balance board events
    BB_EVENT_WII_BB_TOP_LEFT,
    BB_EVENT_WII_BB_TOP_RIGHT,
    BB_EVENT_WII_BB_BOTTOM_LEFT,
    BB_EVENT_WII_BB_BOTTOM_RIGHT,
    BB_EVENT_WII_BB_TEMPERATURE,

    // misc
    BB_EVENT_MISC_BATTERY
};

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

void register_binding(bb_binding b);
void event_manager_setup();
void event_manager_update();