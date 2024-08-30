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

void register_binding(bb_action action, bb_event event, int32_t min, int32_t max, uint8_t pin=0, bool exec_without_controller=true);
void event_manager_setup();
void event_manager_update();