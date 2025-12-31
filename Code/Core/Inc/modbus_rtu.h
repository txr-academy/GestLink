#ifndef __MODBUS_RTU_H
#define __MODBUS_RTU_H

#include <stdint.h>
typedef struct {
    uint16_t raw_sensor_data;      // Reg 0 (PLC 40001)
    uint16_t last_gesture;         // Reg 1 (PLC 40002)
    uint16_t stability_threshold;  // Reg 2 (PLC 40003)
    uint16_t poll_interval_ms;     // Reg 3 (PLC 40004)
} ModbusData_t;

extern volatile ModbusData_t mb_regs;

// Function Prototypes
uint16_t Modbus_CRC16(uint8_t *buf, uint16_t len);
uint16_t Modbus_ProcessRequest(uint8_t *rx_buf, uint8_t *tx_buf, uint16_t rx_len);
#endif
