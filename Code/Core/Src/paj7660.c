#include "PAJ7660.h"

// Private variable to hold the I2C handle
static I2C_HandleTypeDef *paj_i2c;

// --- Private Helper Functions ---

// Write 1 byte to a register
static void PAJ_Write(uint8_t reg, uint8_t value) {
	HAL_I2C_Mem_Write(paj_i2c, PAJ7660_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
}

// Read 1 byte from a register
static uint8_t PAJ_Read(uint8_t reg) {
    uint8_t value = 0;
    HAL_I2C_Mem_Read(paj_i2c, PAJ7660_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
    return value;
}

// --- Public Functions ---

/**
 * @brief  Initializes the PAJ7660 Sensor
 * @param  hi2c Pointer to the I2C handle (e.g., &hi2c1)
 * @retval 1 if success, 0 if failure
 */
uint8_t PAJ7660_Init(I2C_HandleTypeDef *hi2c) {
    paj_i2c = hi2c;


    // 0. Wait for power stabilization
    HAL_Delay(30);

    // 1. Check Connection (Read Part ID) - Optional but good practice
    // Note: We skip the return check here since we rely on the Status check below,
    // but reading it clears any bus glitches.
    PAJ_Read(PAJ7660_REG_PARTID_L);


    // 2. Set Operation Mode to Gesture (0x04) [cite: 1430]
    PAJ_Write(PAJ7660_REG_OP_MODE, 0x04);

    // 3. Enable CPU (0x01) [cite: 1418]
    PAJ_Write(PAJ7660_REG_CPU_EN, 0x01);

    // 4. CRITICAL WAIT: Firmware load from ROM to RAM
    HAL_Delay(250);

    // 5. Verify Sensor Woke Up (Check Bit 0 of Status Reg) [cite: 1417]
    uint8_t status = PAJ_Read(PAJ7660_REG_STATUS);

    if (status & 0x01) {
        return 1; // Success: PowerOn Ready Bit is High
    }

    return 0; // Failure: Sensor is sleeping or not connected
}

/**
 * @brief  Polls the sensor for a new gesture
 * @retval Gesture Code (e.g., GESTURE_1_FINGER) or 0 if none
 */
uint8_t PAJ7660_PollGesture(void) {
    uint8_t gesture_code = GESTURE_NONE;

    // 1. Read Status Register
    uint8_t status = PAJ_Read(PAJ7660_REG_STATUS);

    // 2. Check Bit 1 (Frame Ready) [cite: 1417]
    if (status & 0x02) {

        // 3. Read Raw Gesture Data (Register 0x57)
        uint8_t raw = PAJ_Read(PAJ7660_REG_GESTURE);

        // 4. Decode: The valid data is in Bits 7:3 [cite: 1667]
        gesture_code = (raw >> 3) & 0x1F;

        // 5. CRITICAL: Clear the Interrupt Flag [cite: 458]
        // We must write 0 to Reg 0x04 to tell sensor we read the data
        PAJ_Write(PAJ7660_REG_STATUS, 0x00);
    }

    return gesture_code;
}

void PAJ7660_SetGestureMode(uint8_t mode){
	PAJ_Write(PAJ7660_REG_GESTURE_MODE, mode);
}

uint8_t PAJ7660_ReadGestureMode(void){
	return PAJ_Read(PAJ7660_REG_GESTURE_MODE);
}


