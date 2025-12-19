#ifndef PAJ7660_H
#define PAJ7660_H

#include "stm32f2xx_hal.h" // Change to f4xx/f1xx if you switch boards later

// --- I2C Configuration ---
// The sensor address is 0x68. HAL expects it left-shifted by 1 (0xD0).
#define PAJ7660_ADDR            (0x68 << 1)

// --- Register Addresses ---
#define PAJ7660_REG_BANK		0x7F
#define PAJ7660_REG_PARTID_L    0x00
#define PAJ7660_REG_PARTID_H    0x01
#define PAJ7660_REG_STATUS      0x04
#define PAJ7660_REG_CPU_EN      0x0A
#define PAJ7660_REG_OP_MODE     0x10
#define PAJ7660_REG_GESTURE     0x57
#define PAJ7660_REG_GESTURE_MODE 0x22

// --- Gesture Modes (Reg 0x22) ---
#define MODE_THUMB              0x02
#define MODE_CURSOR             0x04
#define MODE_COMBINED           0x05

// --- Gesture Codes (Decoded) ---
// These match the values returned after shifting (raw >> 3)
#define GESTURE_NONE            0x00
#define GESTURE_1_FINGER        0x01
#define GESTURE_2_FINGER        0x02
#define GESTURE_3_FINGER        0x03
#define GESTURE_4_FINGER        0x04
#define GESTURE_5_FINGER        0x05
#define GESTURE_CW              0x06 // Clockwise Rotate
#define GESTURE_CCW             0x07 // Counter-Clockwise
#define GESTURE_LEFT            0x08
#define GESTURE_RIGHT           0x09
#define FINGER_1_PUSH           0x13

// --- Function Prototypes ---
uint8_t PAJ7660_Init(I2C_HandleTypeDef *hi2c);
uint8_t PAJ7660_PollGesture(void);
void PAJ7660_SetGestureMode(uint8_t mode);
uint8_t PAJ7660_ReadGestureMode(void);

#endif
