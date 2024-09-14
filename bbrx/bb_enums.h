#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <algorithm>

#include "bb_enums.h"

// split string into parts using delimiter
// from https://stackoverflow.com/a/14266139
inline std::vector<std::string> split(std::string& s, std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);

    return tokens;
}
 
/*
 * Hell Macro which defines a reflective enum called name, along with members provided as ..., defining the following things in the scope of invocation:
 * - <name>_to_string(int)              converts a member of name to a string version of that member
 * - <name>_to_enum(std::string)        converts a string to the matching member of name 
 *
 * It also creates a few internal use objects:
 * - <name>Members                      a vector, where each element is a name member
 * - <name>Strings                      a vector, where each element is the string version of each name member
 * - <name>StdString                    a big string with all the name members in comma-space separated format
 * - <name>delim                        basically just ", "
 * - <name>_to_enum_id(std::string)     a function which converts a string to the integer version of the matching name member
 * 
 * note that the indexes of <name>Members and <name>Strings should match up
 *
 * This depends on the following stdlibs: vector, iostream, algorithm
 *
 * This is heavily based on a macro provided in this blog post by Chloé Lourseyre:
 * https://belaycpp.com/2021/08/24/best-ways-to-convert-an-enum-to-a-string/
 */
#define ENUM_MACRO(name, ...)\
    enum name { __VA_ARGS__ };\
    inline std::string name##StdString = std::string(#__VA_ARGS__);                            /* single string with comma separated enum members */ \
    inline std::string name##delim = ", ";                                                     /* delimiter */ \
    inline std::vector<std::string> name##Strings = split(name##StdString, name##delim);       /* split string into vector of strings where each string is an enum member */ \
    inline std::vector<name> name##Members = { __VA_ARGS__ };                                  /* create vector where each element is an enum member, so the enum ID == index */ \
    inline std::string name##_to_string(int value) { return name##Strings[value]; }              /* function to convert an enum to it's string version */ \
    inline int name##_to_enum_id(std::string value) { std::vector<std::string>::iterator found = std::find(name##Strings.begin(), name##Strings.end(), value); return (found == name##Strings.end()) ? -1 : found - name##Strings.begin(); } \
    inline name name##_to_enum(std::string value)  { return (name) name##_to_enum_id(value); }




/*
 * As a result of ENUM_MACRO and the bb_ enums defined below, the following functions are
 * available for reflecting enums:
 * 
 * - std::string bb_action_to_string(int value);
 * - int bb_action_to_enum(std::string value);
 * - std::string bb_event_to_string(int value);
 * - int bb_event_to_enum(std::string value);
*/


/**
 * @brief Enum containing supported receiver actions
 * 
 */
ENUM_MACRO(bb_action,
    BB_ACTION_TEST,
    BB_ACTION_SERVO,
    BB_ACTION_SPEED_UP,
    BB_ACTION_SPEED_DOWN,
    BB_ACTION_SPEED_SET,
    BB_ACTION_BRAKE
);

/**
 * @brief Enum containing supported gamepad events
 * 
 */
ENUM_MACRO(bb_event,

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

    // dpad buttons
    BB_EVENT_DPAD_UP,
    BB_EVENT_DPAD_DOWN,
    BB_EVENT_DPAD_LEFT,
    BB_EVENT_DPAD_RIGHT,

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
);