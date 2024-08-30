#pragma once

#include <Bluepad32.h>
#include <functional>

/**
Sets up Bluepad32.  Should only be called once.
*/
void controller_setup();

/**
Read the current controller input, and pass it to a callback
*/
void controller_handle(std::function<void(ControllerPtr controller)> callback);

/**
 * @brief Indicates whether at least one controller is connected
 * 
 * @return true one or more controllers are connected
 * @return false no controllers are connected
 */
bool controller_connected();