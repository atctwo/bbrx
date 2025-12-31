#include "feedback.h"

void ControllerFeedback::setLEDColor(ControllerPtr controller, uint8_t r, uint8_t g, uint8_t b) {
    if (controller == nullptr) return;
    
    controller->setColorLED(r, g, b);
    logd(LOG_TAG, "Set LED color: R=%d G=%d B=%d", r, g, b);
}

void ControllerFeedback::setRumble(ControllerPtr controller, uint8_t force, uint8_t duration) {
    if (controller == nullptr) return;
    
    controller->setRumble(force, force);
    logd(LOG_TAG, "Set rumble: force=%d duration=%d", force, duration);
    
    if (duration > 0) {
        // Schedule rumble off after duration (would need task/timer implementation)
        // For now, caller must handle timing
    }
}

void ControllerFeedback::playEffect(ControllerPtr controller, const char* effect_name) {
    if (controller == nullptr) return;
    
    logd(LOG_TAG, "Playing effect: %s", effect_name);
    
    if (strcmp(effect_name, "connect") == 0) {
        effectConnectionSuccess(controller);
    } else if (strcmp(effect_name, "disconnect") == 0) {
        effectConnectionLost(controller);
    } else if (strcmp(effect_name, "config_loaded") == 0) {
        effectConfigLoaded(controller);
    } else if (strcmp(effect_name, "error") == 0) {
        effectError(controller);
    } else if (strcmp(effect_name, "warning") == 0) {
        effectWarning(controller);
    }
}

void ControllerFeedback::effectConnectionSuccess(ControllerPtr controller) {
    setLEDColor(controller, 0, 255, 0); // Green
    setRumble(controller, 128, 100);
}

void ControllerFeedback::effectConnectionLost(ControllerPtr controller) {
    setLEDColor(controller, 255, 0, 0); // Red
    setRumble(controller, 255, 200);
}

void ControllerFeedback::effectConfigLoaded(ControllerPtr controller) {
    setLEDColor(controller, 0, 206, 209); // Cyan
    setRumble(controller, 64, 50);
}

void ControllerFeedback::effectError(ControllerPtr controller) {
    setLEDColor(controller, 255, 0, 0); // Red
    setRumble(controller, 255, 500);
}

void ControllerFeedback::effectWarning(ControllerPtr controller) {
    setLEDColor(controller, 255, 165, 0); // Orange
    setRumble(controller, 128, 150);
}