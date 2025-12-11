// gesture_utility.h

#ifndef __GESTURE_UTILITY_H
#define __GESTURE_UTILITY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h" // Essential for HAL types (GPIO_TypeDef, uint16_t, etc.)

/* Exported Constants and Macros ---------------------------------------------*/

// Define the single GPIO Port used for all 5 LEDs (PD1 through PD5)
#define LED_GPIO_PORT GPIOD

// Define the size of the history queue for stable state implementation
#define GESTURE_QUEUE_SIZE 10

/* Exported Variables (External Declarations) --------------------------------*/

// CRITICAL: Declaration of the array holding the PIN numbers (PD1 to PD5)
// Defined in gesture_utility.c
extern const uint16_t LED_PINS[];

// Declaration of the simulation/history variables (Defined in read_gesture_id.c)
extern uint8_t gesture_history_queue[GESTURE_QUEUE_SIZE];
extern int gesture_queue_index;

/* Exported Functions Prototypes ---------------------------------------------*/

// Function to provide visual feedback on the 5 LEDs
void gesture_feedback(uint8_t gesture_id);

// Function to read the next gesture ID (will eventually include stability logic)
uint8_t read_gesture_id(void);

#ifdef __cplusplus
}
#endif

#endif /* __GESTURE_UTILITY_H */
