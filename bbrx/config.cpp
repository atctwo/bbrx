#include <Arduino.h>
#include <Bluepad32.h>
#include "log.h"
#include "config.h"

#include <vector>
#include "event_manager.h"

#define LOG_TAG "config"

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