#pragma once

#include <Bluepad32.h>
#include "config.h"
#include "log.h"

#define LOG_TAG "feedback"

class ControllerFeedback {
public:
    static void setLEDColor(ControllerPtr controller, uint8_t r, uint8_t g, uint8_t b);
    static void setRumble(ControllerPtr controller, uint8_t force, uint8_t duration);
    static void playEffect(ControllerPtr controller, const char* effect_name);
    
    // Predefined effects
    static void effectConnectionSuccess(ControllerPtr controller);
    static void effectConnectionLost(ControllerPtr controller);
    static void effectConfigLoaded(ControllerPtr controller);
    static void effectError(ControllerPtr controller);
    static void effectWarning(ControllerPtr controller);
};