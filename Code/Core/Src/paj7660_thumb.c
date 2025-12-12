/*
 * paj7660_thumb.c
 *
 *  Created on: Dec 12, 2025
 *      Author: jyothish.tr
 */


#include "paj7660_thumb.h"

ThumbState_t PAJ7660_ReadThumbState(I2C_HandleTypeDef *hi2c) {
    uint8_t data_up = 0;
    uint8_t data_down = 0;
    HAL_StatusTypeDef status;

    // 1. BANK SWITCHING SAFETY [cite: 1341]
    // In complex systems, we always enforce the Bank before critical reads.
    // Writing 0x00 to register 0x7F selects Bank 0.
    uint8_t bank_sel = 0x00;
    status = HAL_I2C_Mem_Write(hi2c, PAJ7660_I2C_ADDR, PAJ7660_REG_BANK_SEL,
                               I2C_MEMADD_SIZE_8BIT, &bank_sel, 1, 100);
    if (status != HAL_OK) return THUMB_ERROR;

    // 2. READ THUMB UP (Register 0x57)
    status = HAL_I2C_Mem_Read(hi2c, PAJ7660_I2C_ADDR, PAJ7660_REG_THUMB_UP,
                              I2C_MEMADD_SIZE_8BIT, &data_up, 1, 100);
    if (status != HAL_OK) return THUMB_ERROR;

    // 3. READ THUMB DOWN (Register 0x5B)
    status = HAL_I2C_Mem_Read(hi2c, PAJ7660_I2C_ADDR, PAJ7660_REG_THUMB_DOWN,
                              I2C_MEMADD_SIZE_8BIT, &data_down, 1, 100);
    if (status != HAL_OK) return THUMB_ERROR;

    // 4. PARSE FLAGS
    // The datasheet confirms these are single-bit flags at Bit 0.
    uint8_t is_up   = (data_up   & 0x01);
    uint8_t is_down = (data_down & 0x01);

    // 5. PRIORITY LOGIC
    // If both are somehow 1 (unlikely but possible noise), prioritize one or return Error.
    if (is_up && is_down) {
        return THUMB_ERROR; // Or handle as per your preference
    } else if (is_up) {
        return THUMB_UP;
    } else if (is_down) {
        return THUMB_DOWN;
    }

    return THUMB_NONE;
}
