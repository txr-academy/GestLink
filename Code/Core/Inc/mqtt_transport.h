/**
 * @file mqtt_transport.h
 * @brief MQTT Client API for GestLink
 * 
 * Provides functions to connect to an MQTT broker and publish gesture data.
 */

#ifndef __MQTT_TRANSPORT_H
#define __MQTT_TRANSPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* -------------------------------------------------------------------------- */
/*                          Return Codes                                      */
/* -------------------------------------------------------------------------- */
#define MQTT_OK                 0
#define MQTT_ERR_SOCKET        -1
#define MQTT_ERR_CONNECT       -2
#define MQTT_ERR_PUBLISH       -3
#define MQTT_ERR_NOT_CONNECTED -4

/* -------------------------------------------------------------------------- */
/*                          Public API Functions                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Connect to the MQTT broker
 * 
 * Opens a TCP socket, connects to the broker, and performs MQTT handshake.
 * 
 * @return MQTT_OK on success, negative error code on failure
 */
int gestlink_mqtt_connect(void);

/**
 * @brief Disconnect from the MQTT broker
 * 
 * Sends DISCONNECT packet and closes the socket.
 */
void gestlink_mqtt_disconnect(void);

/**
 * @brief Check if connected to broker
 * 
 * @return 1 if connected, 0 if not
 */
int gestlink_mqtt_is_connected(void);

/**
 * @brief Publish a gesture to the MQTT broker
 * 
 * Sends the gesture as a JSON payload: {"gesture_id":X,"gesture_name":"..."}
 * 
 * @param gesture_id The gesture code from PAJ7660 sensor
 * @return MQTT_OK on success, negative error code on failure
 */
int gestlink_mqtt_publish_gesture(uint8_t gesture_id);

/**
 * @brief Send MQTT keepalive (PINGREQ)
 * 
 * Call this periodically to maintain connection.
 * 
 * @return MQTT_OK on success, negative error code on failure
 */
int gestlink_mqtt_send_keepalive(void);

/**
 * @brief Get gesture name string
 * 
 * @param gesture_id The gesture code
 * @return Static string with gesture name
 */
const char* gestlink_mqtt_get_gesture_name(uint8_t gesture_id);

/* -------------------------------------------------------------------------- */
/*                          Transport Layer (for Paho)                        */
/* -------------------------------------------------------------------------- */

int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen);
int transport_getdata(int sock, unsigned char* buf, int count);

#ifdef __cplusplus
}
#endif

#endif /* __MQTT_TRANSPORT_H */
