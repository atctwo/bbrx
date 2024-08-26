#pragma once

/*
 * functions to print logging output
*/

#include <stdarg.h>
#include <stdio.h>
#include <Arduino.h>

// #define LOG_OUTPUT USBSerial
#define LOG_OUTPUT Serial

// #define logd(tag, fmt, ...) (void) 0;
#define logd(tag, fmt, ...) LOG_OUTPUT.printf("[%s] " fmt "\n", tag, ## __VA_ARGS__)

// #define logi(tag, fmt, ...) (void) 0;
#define logi(tag, fmt, ...) LOG_OUTPUT.printf("[%s] " fmt "\n", tag, ## __VA_ARGS__)

// #define loge(tag, fmt, ...) (void) 0;
#define logw(tag, fmt, ...) LOG_OUTPUT.printf("[%s] " fmt "\n", tag, ## __VA_ARGS__)

// #define loge(tag, fmt, ...) (void) 0;
#define loge(tag, fmt, ...) LOG_OUTPUT.printf("[%s] " fmt "\n", tag, ## __VA_ARGS__)