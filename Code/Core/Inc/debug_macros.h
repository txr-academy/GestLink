#ifndef DEBUG_MACROS_H
#define DEBUG_MACROS_H

#include <stdio.h>
#include <string.h>
#include "main.h"

typedef enum {
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_USER
} LogLevel_t;

#define DEBUG_MAX_LENGTH 128

#define DEBUG_LOG(level, format, ...) \
    do { \
        char debug_buffer[DEBUG_MAX_LENGTH]; \
        char *tag; \
        /* Logic to decide the tag based on level */ \
        switch (level) { \
            case LOG_LEVEL_ERROR: tag = "[ERROR]"; break; \
            case LOG_LEVEL_WARN:  tag = "[WARN]";  break; \
            case LOG_LEVEL_INFO:  tag = "[INFO]";  break; \
            case LOG_LEVEL_USER:  tag = "[USER]";  break; \
            default:              tag = "[DBUG]";  break; \
        } \
        int len = snprintf(debug_buffer, DEBUG_MAX_LENGTH, "%s " format "\r\n", tag, ##__VA_ARGS__); \
        extern UART_HandleTypeDef huart3; \
        HAL_UART_Transmit(&huart3, (uint8_t*)debug_buffer, len, HAL_MAX_DELAY); \
    } while(0)

#endif // DEBUG_MACROS_Hs
