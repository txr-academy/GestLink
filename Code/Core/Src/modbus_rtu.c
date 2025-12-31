#include "modbus_rtu.h"

// Define the global variable shared across tasks
volatile ModbusData_t mb_regs = {0, 0, 3, 100};

/**
 * @brief Standard Modbus CRC16 Calculation
 */
uint16_t Modbus_CRC16(uint8_t *buf, uint16_t len) {
    uint16_t crc = 0xFFFF;
    for (uint16_t pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos];
        for (int i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/**
 * @brief Processes the Modbus request and prepares a response
 * @param rx_buf: Pointer to received data
 * @param tx_buf: Pointer to buffer where response will be stored
 * @param rx_len: Length of received data (usually 8 bytes)
 * @return Length of the response packet to be transmitted
 */
uint16_t Modbus_ProcessRequest(uint8_t *rx_buf, uint8_t *tx_buf, uint16_t rx_len) {
    uint8_t slave_id = 1;


    if (rx_buf[0] != slave_id) return 0;

    // 2. Validate CRC of incoming packet
    uint16_t received_crc = (rx_buf[rx_len - 1] << 8) | rx_buf[rx_len - 2];
    if (Modbus_CRC16(rx_buf, rx_len - 2) != received_crc) return 0;

    uint8_t function_code = rx_buf[1];
    uint16_t start_addr = (rx_buf[2] << 8) | rx_buf[3];

    // --- Function 03: Read Holding Registers ---
    if (function_code == 0x03) {
        uint16_t num_regs = (rx_buf[4] << 8) | rx_buf[5];

        tx_buf[0] = slave_id;
        tx_buf[1] = 0x03;
        tx_buf[2] = num_regs * 2;

        for (int i = 0; i < num_regs; i++) {
            uint16_t val = 0;

            switch (start_addr + i) {
                case 0: val = mb_regs.raw_sensor_data; break;      // PLC 40001
                case 1: val = mb_regs.last_gesture; break;         // PLC 40002
                case 2: val = mb_regs.stability_threshold; break;  // PLC 40003
                case 3: val = mb_regs.poll_interval_ms; break;     // PLC 40004
                default: val = 0xFFFF; break; // Error value for invalid address
            }
            tx_buf[3 + (i * 2)] = (val >> 8) & 0xFF; // High Byte
            tx_buf[4 + (i * 2)] = val & 0xFF;        // Low Byte
        }

        uint16_t crc = Modbus_CRC16(tx_buf, 3 + (num_regs * 2));
        tx_buf[3 + (num_regs * 2)] = crc & 0xFF;        // CRC Low
        tx_buf[4 + (num_regs * 2)] = (crc >> 8) & 0xFF; // CRC High

        return 5 + (num_regs * 2);
    }

    // --- Function 06: Write Single Register ---
    else if (function_code == 0x06) {
        uint16_t write_val = (rx_buf[4] << 8) | rx_buf[5];

        // Only allow writing to threshold and interval
        if (start_addr == 2) mb_regs.stability_threshold = write_val;
        else if (start_addr == 3) mb_regs.poll_interval_ms = write_val;
        else return 0; // Don't respond if writing to Read-Only registers

        // Echo the 8-byte request back as acknowledgment
        for(int i = 0; i < 8; i++)
        	tx_buf[i] = rx_buf[i];
        return 8;
    }

    return 0; // Function code not supported
}
