#include "gesture_utility.h"
#include<stdio.h>

#define LED_GPIO_PORT GPIOD

// This is the ONLY place the LED_PINS array is fully DEFINED.
// It uses GPIO_PIN_1 through GPIO_PIN_5 on Port D.
const uint16_t LED_PINS[] = {
    GPIO_PIN_1, // L1 (Gesture 1)
    GPIO_PIN_2, // L2 (Gesture 2)
    GPIO_PIN_3, // L3 (Gesture 3)
    GPIO_PIN_4, // L4 (Gesture 4)
    GPIO_PIN_5  // L5 (Gesture 5)
};

#define NUM_LEDS 5

/**
 * @brief Clears all 5 LEDs (sets them to the OFF state) by iterating through the mixed-port array.
 */
static void clear_all_leds(void) {
    for (int i = 0; i < NUM_LEDS; i++) {
    	HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PINS[i], GPIO_PIN_RESET);
    }
}

/**
 * @brief Implements static count feedback for gestures 1 through 5.
 * @param gesture_id The ID of the detected gesture (1-5 for counts).
 */
void gesture_feedback(uint8_t gesture_id) {

    // 1. First, clear any previous LED state
    clear_all_leds();

    // 2. Check for the static count gestures (ID 1 through 5)
    if (gesture_id >= 1 && gesture_id <= NUM_LEDS) {

        int leds_to_light = (int)gesture_id;

        // Loop only up to the required number of LEDs (e.g., if ID=3, loop runs for i=0, 1, 2)
        for (int i = 0; i < leds_to_light; i++) {

        	HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PINS[i], GPIO_PIN_SET);
        }

        // Add Debug Print (using the configured huart3 handle)
        char dbg_buffer[40];
        int len = snprintf(dbg_buffer, sizeof(dbg_buffer), "[INFO] Feedback: Count %d displayed.\r\n", gesture_id);
        extern UART_HandleTypeDef huart3; // Access the handle defined in main.c
        HAL_UART_Transmit(&huart3, (uint8_t*)dbg_buffer, len, HAL_MAX_DELAY);

    } else {
        // Logic for other gestures (Swipe, Grab, Tap) will go here.
    }
}
