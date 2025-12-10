// read_gesture_id.c

#include "gesture_utility.h"

#define GESTURE_QUEUE_SIZE 10
// Define the simulation variables (Global definition for the array and index)
uint8_t gesture_history_queue[GESTURE_QUEUE_SIZE] = {
    1, 1, 1,
    2, 2, 2,
    5, 5, 5,
    0  // Stop/No gesture
};

int gesture_queue_index = 0;

/**
 * @brief Reads the next simulated gesture ID from the global array.
 * * In the final version, this will be replaced by I2C/Sensor reading.
 * As of now, it cycles through the defined 'gesture_history_queue'.
 * * @retval The current gesture ID from the simulated queue.
 */
uint8_t read_gesture_id(void) {

    // Check if we have run out of defined gestures
    if (gesture_queue_index >= GESTURE_QUEUE_SIZE) {
        // Stop the simulation once the end is reached
        return 0;
    }

    // Get the current gesture ID from the queue
    uint8_t current_id = gesture_history_queue[gesture_queue_index];

    // Increment the index for the next call
    gesture_queue_index++;

    // Return the ID to main.c
    return current_id;
}
