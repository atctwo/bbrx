#pragma once

/*
 * functions to print logging output
*/

#include <stdarg.h>
#include <stdio.h>
#include <Arduino.h>

// #define LOG_OUTPUT USBSerial
#define LOG_OUTPUT Serial

#define NEWLINE "\n"
#define ANSI_ENABLE // uncomment to disable ansi colour coding
#define ANSI_ESC "\x1b"

#ifdef ANSI_ENABLE
    #define ANSI_BOLD                   ANSI_ESC "[1m"
    #define ANSI_DIM                    ANSI_ESC "[2m"
    #define ANSI_ITALIC                 ANSI_ESC "[3m"
    #define ANSI_UNDERLINE              ANSI_ESC "[4m"
    #define ANSI_BLINK                  ANSI_ESC "[5m"
    #define ANSI_REVERSE                ANSI_ESC "[7m"
    #define ANSI_INVISIBLE              ANSI_ESC "[8m"
    #define ANSI_STRIKETHROUGH          ANSI_ESC "[9m"

    #define ANSI_FG_BLACK               ANSI_ESC "[30m"
    #define ANSI_FG_RED                 ANSI_ESC "[31m"
    #define ANSI_FG_GREEN               ANSI_ESC "[32m"
    #define ANSI_FG_YELLOW              ANSI_ESC "[33m"
    #define ANSI_FG_BLUE                ANSI_ESC "[34m"
    #define ANSI_FG_MAGENTA             ANSI_ESC "[35m"
    #define ANSI_FG_CYAN                ANSI_ESC "[36m"
    #define ANSI_FG_WHITE               ANSI_ESC "[37m"
    #define ANSI_FG_DEFAULT             ANSI_ESC "[39m"

    #define ANSI_FG_BLACK_BRIGHT        ANSI_ESC "[90m"
    #define ANSI_FG_RED_BRIGHT          ANSI_ESC "[91m"
    #define ANSI_FG_GREEN_BRIGHT        ANSI_ESC "[92m"
    #define ANSI_FG_YELLOW_BRIGHT       ANSI_ESC "[93m"
    #define ANSI_FG_BLUE_BRIGHT         ANSI_ESC "[94m"
    #define ANSI_FG_MAGENTA_BRIGHT      ANSI_ESC "[95m"
    #define ANSI_FG_CYAN_BRIGHT         ANSI_ESC "[96m"
    #define ANSI_FG_WHITE_BRIGHT        ANSI_ESC "[97m"

    #define ANSI_BG_BLACK               ANSI_ESC "[40m"
    #define ANSI_BG_RED                 ANSI_ESC "[41m"
    #define ANSI_BG_GREEN               ANSI_ESC "[42m"
    #define ANSI_BG_YELLOW              ANSI_ESC "[43m"
    #define ANSI_BG_BLUE                ANSI_ESC "[44m"
    #define ANSI_BG_MAGENTA             ANSI_ESC "[45m"
    #define ANSI_BG_CYAN                ANSI_ESC "[46m"
    #define ANSI_BG_WHITE               ANSI_ESC "[47m"
    #define ANSI_BG_DEFAULT             ANSI_ESC "[49m"

    #define ANSI_BG_BLACK_BRIGHT        ANSI_ESC "[100m"
    #define ANSI_BG_RED_BRIGHT          ANSI_ESC "[101m"
    #define ANSI_BG_GREEN_BRIGHT        ANSI_ESC "[102m"
    #define ANSI_BG_YELLOW_BRIGHT       ANSI_ESC "[103m"
    #define ANSI_BG_BLUE_BRIGHT         ANSI_ESC "[104m"
    #define ANSI_BG_MAGENTA_BRIGHT      ANSI_ESC "[105m"
    #define ANSI_BG_CYAN_BRIGHT         ANSI_ESC "[106m"
    #define ANSI_BG_WHITE_BRIGHT        ANSI_ESC "[107m"

    #define ANSI_RESET                  ANSI_ESC "[0m"
#else
    #define ANSI_BOLD           ""
    #define ANSI_DIM            ""
    #define ANSI_ITALIC         ""
    #define ANSI_UNDERLINE      ""
    #define ANSI_BLINK          ""
    #define ANSI_REVERSE        ""
    #define ANSI_INVISIBLE      ""
    #define ANSI_STRIKETHROUGH  ""

    #define ANSI_FG_BLACK       ""
    #define ANSI_FG_RED         ""
    #define ANSI_FG_GREEN       ""
    #define ANSI_FG_YELLOW      ""
    #define ANSI_FG_BLUE        ""
    #define ANSI_FG_MAGENTA     ""
    #define ANSI_FG_CYAN        ""
    #define ANSI_FG_WHITE       ""
    #define ANSI_FG_DEFAULT     ""

    #define ANSI_BG_BLACK       ""
    #define ANSI_BG_RED         ""
    #define ANSI_BG_GREEN       ""
    #define ANSI_BG_YELLOW      ""
    #define ANSI_BG_BLUE        ""
    #define ANSI_BG_MAGENTA     ""
    #define ANSI_BG_CYAN        ""
    #define ANSI_BG_WHITE       ""
    #define ANSI_BG_DEFAULT     ""

    #define ANSI_RESET       ""
#endif

#define logv(tag, fmt, ...) (void) 0;
// #define logv(tag, fmt, ...) LOG_OUTPUT.printf(ANSI_FG_PURPLE "[v]" ANSI_RESET "[%s] " ANSI_DIM fmt ANSI_RESET NEWLINE, tag, ## __VA_ARGS__)

// #define logd(tag, fmt, ...) (void) 0;
#define logd(tag, fmt, ...) LOG_OUTPUT.printf(ANSI_FG_CYAN "[d] " ANSI_RESET "[%s] " ANSI_DIM fmt ANSI_RESET NEWLINE, tag, ## __VA_ARGS__)

// #define logi(tag, fmt, ...) (void) 0;
#define logi(tag, fmt, ...) LOG_OUTPUT.printf(ANSI_FG_GREEN "[i] " ANSI_RESET "[%s] " fmt NEWLINE, tag, ## __VA_ARGS__)

// #define loge(tag, fmt, ...) (void) 0;
#define logw(tag, fmt, ...) LOG_OUTPUT.printf(ANSI_FG_YELLOW "[w] " ANSI_RESET "[%s] " fmt NEWLINE, tag, ## __VA_ARGS__)

// #define loge(tag, fmt, ...) (void) 0;
#define loge(tag, fmt, ...) LOG_OUTPUT.printf(ANSI_FG_RED "[e] " ANSI_RESET "[%s] " fmt NEWLINE, tag, ## __VA_ARGS__)