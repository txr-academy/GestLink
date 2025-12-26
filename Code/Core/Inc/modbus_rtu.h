#ifndef __MODBUS_RTU_H
#define __MODBUS_RTU_H

#include "main.h"

// Modbus Register Structure
typedef struct {
    uint16_t last_gesture;         // Changed from stable_gesture
    uint16_t stability_threshold;  // Changed from stability_limit
    uint16_t poll_interval_ms;     // Changed from poll_delay
} ModbusData_t;
// Extern the global registers so they can be seen in main.c
extern volatile ModbusData_t mb_regs;

// Function Prototypes
uint16_t Modbus_CRC16(uint8_t *buf, uint16_t len);

#endif
