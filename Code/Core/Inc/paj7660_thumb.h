/*
 * paj7660_thumb.h
 *
 *  Created on: Dec 12, 2025
 *      Author: jyothish.tr
 */

#ifndef INC_PAJ7660_THUMB_H_
#define INC_PAJ7660_THUMB_H_

#include "stm32f2xx_hal.h"

#define PAJ7660_I2C_ADDR        (0x68 << 1)

/* Register Definitions */
#define PAJ7660_REG_BANK_SEL    0x7F
#define PAJ7660_REG_THUMB_UP    0x57
#define PAJ7660_REG_THUMB_DOWN  0x5B

/* Mode Definitions */
#define PAJ7660_MODE_THUMB      0x02

/* Return Type Enumeration */
typedef enum {
    THUMB_NONE = 0,
    THUMB_UP,
    THUMB_DOWN,
    THUMB_ERROR
} ThumbState_t;

ThumbState_t PAJ7660_ReadThumbState(I2C_HandleTypeDef *hi2c);

#endif /* INC_PAJ7660_THUMB_H_ */
