#include "PAJ7660.h"
#include <stdio.h>
// Private variable to hold the I2C handle
static I2C_HandleTypeDef *paj_i2c;

// --- Private Helper Functions ---

// Write 1 byte to a register
static uint8_t PAJ_Write(uint8_t reg, uint8_t value) {
	HAL_StatusTypeDef ret;
	ret = HAL_I2C_Mem_Write(paj_i2c, PAJ7660_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
	if(ret != HAL_OK){
		return 1;
	}
	return 0;
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
    HAL_Delay(50);

    if (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY) {
            printf("I2C Bus Busy. Attempting Recovery...\r\n");

            // A. De-Init the Driver (Release pins)
            HAL_I2C_DeInit(hi2c);

            // B. Manual Recovery (Toggle Pins)
            I2C_ClearBus();

            // C. Re-Init the Driver (Restore I2C functionality)
            HAL_I2C_Init(hi2c);
    }
	// 1. Force Bank 0
	PAJ_Write(0x7F, 0x00);

	HAL_Delay(10);

    uint8_t id_l = PAJ_Read(PAJ7660_REG_PARTID_L); // 0x00
    uint8_t id_h = PAJ_Read(PAJ7660_REG_PARTID_H); // 0x01
//    if (id_l != 0x60 || id_h != 0x76) {
//        // If you have printf enabled, uncomment this:
//         printf("PAJ7660 Error: ID Mismatch (Got 0x%02X%02X)\r\n", id_h, id_l);
//        return 0; // Stop here! Don't wait 250ms for a ghost sensor.
//    }


	uint8_t current_status = PAJ_Read(PAJ7660_REG_STATUS);
	printf("%d\r\n", current_status);

	// Check Bit 0 (PowerOn Ready)
	if (current_status != 0x00) {
		// It is ALREADY awake! No need to re-initialize.
		// Just clear any old interrupts and return success.
		PAJ_Write(PAJ7660_REG_STATUS, 0x00);
		return 1;
	}
    PAJ_Write(PAJ7660_REG_CPU_EN, 0x00);
    HAL_Delay(10);
    PAJ_Write(PAJ7660_REG_OP_MODE, 0x00);
    HAL_Delay(10);
    // 1. Check Connection (Read Part ID) - Optional but good practice
    // Note: We skip the return check here since we rely on the Status check below,
    // but reading it clears any bus glitches.
//    uint8_t id_l = PAJ_Read(PAJ7660_REG_PARTID_L); // 0x00
//    uint8_t id_h = PAJ_Read(PAJ7660_REG_PARTID_H); // 0x01
//    if (id_l != 0x60 || id_h != 0x76) {
//        // If you have printf enabled, uncomment this:
//         printf("PAJ7660 Error: ID Mismatch (Got 0x%02X%02X)\r\n", id_h, id_l);
//        return 0; // Stop here! Don't wait 250ms for a ghost sensor.
//    }
    // 2. Set Operation Mode to Gesture (0x04) [cite: 1430]
    if(PAJ_Write(PAJ7660_REG_OP_MODE, 0x04) == 1) printf("Error while writing");

    // 3. Enable CPU (0x01) [cite: 1418]
    if(PAJ_Write(PAJ7660_REG_CPU_EN, 0x01) == 1) printf("Error while writing CPU EN");

    // 4. CRITICAL WAIT: Firmware load from ROM to RAM
    HAL_Delay(400);

	// 5. Verify Sensor Woke Up (Check Bit 0 of Status Reg) [cite: 1417]
    uint8_t status = PAJ_Read(PAJ7660_REG_STATUS);

    if (status != 0) {
        return 1; // Success: PowerOn Ready Bit is High
    }
    printf("Failure\r\n");
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
//
//void PAJ7660_SetGestureMode(uint8_t mode){
//	PAJ_Write(PAJ7660_REG_GESTURE_MODE, mode);
//}
//
//uint8_t PAJ7660_ReadGestureMode(void){
//	return PAJ_Read(PAJ7660_REG_GESTURE_MODE);
//}
//

