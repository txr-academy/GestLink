/**
 * @file mqtt_transport.c
 * @brief MQTT Client for GestLink - Gesture Publishing over MQTT
 */

#include "mqtt_transport.h"
#include "lwip/sockets.h"
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include "MQTTPacket.h"
#include "paj7660.h"
#include <string.h>
#include <stdio.h>

/* -------------------------------------------------------------------------- */
/*                          Configuration                                     */
/* -------------------------------------------------------------------------- */
#define MQTT_BROKER_IP       "34.243.217.54"   /* HiveMQ public broker */
#define MQTT_BROKER_PORT     1883
#define MQTT_CLIENT_ID       "GestLink_Device"
#define MQTT_TOPIC           "gestlink/gesture"
#define MQTT_KEEPALIVE_SEC   60
#define MQTT_BUFFER_SIZE     200

/* -------------------------------------------------------------------------- */
/*                          Private Variables                                 */
/* -------------------------------------------------------------------------- */
static int mqtt_socket = -1;
static unsigned char mqtt_buf[MQTT_BUFFER_SIZE];

/* -------------------------------------------------------------------------- */
/*                          Transport Layer (for Paho)                        */
/* -------------------------------------------------------------------------- */

int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen) {
    return send(sock, buf, buflen, 0);
}

int transport_getdata(int sock, unsigned char* buf, int count) {
    return recv(sock, buf, count, 0);
}

/* -------------------------------------------------------------------------- */
/*                          Helper Functions                                  */
/* -------------------------------------------------------------------------- */

const char* gestlink_mqtt_get_gesture_name(uint8_t gesture_id) {
    switch(gesture_id) {
        case GESTURE_1_FINGER: return "1_finger";
        case GESTURE_2_FINGER: return "2_finger";
        case GESTURE_3_FINGER: return "3_finger";
        case GESTURE_4_FINGER: return "4_finger";
        case GESTURE_5_FINGER: return "5_finger";
        case GESTURE_LEFT:     return "swipe_left";
        case GESTURE_RIGHT:    return "swipe_right";
        case GESTURE_CW:       return "rotate_cw";
        case GESTURE_CCW:      return "rotate_ccw";
        case FINGER_1_PUSH:    return "push";
        default:               return "unknown";
    }
}

/* -------------------------------------------------------------------------- */
/*                          Public API Functions                              */
/* -------------------------------------------------------------------------- */

int gestlink_mqtt_connect(void) {
    struct sockaddr_in servaddr;
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    int len;

    /* Close existing socket if any */
    if (mqtt_socket >= 0) {
        close(mqtt_socket);
        mqtt_socket = -1;
    }

    // printf("[MQTT] Creating socket...\r\n");

    /* Create socket */
    mqtt_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (mqtt_socket < 0) {
        // printf("[MQTT] Socket creation failed!\r\n");
        return MQTT_ERR_SOCKET;
    }
    // printf("[MQTT] Socket created: %d\r\n", mqtt_socket);

    /* Connect to broker */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MQTT_BROKER_PORT);
    servaddr.sin_addr.s_addr = inet_addr(MQTT_BROKER_IP);

    // printf("[MQTT] Connecting to %s:%d...\r\n", MQTT_BROKER_IP, MQTT_BROKER_PORT);

    if (connect(mqtt_socket, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        // printf("[MQTT] TCP connection failed!\r\n");
        close(mqtt_socket);
        mqtt_socket = -1;
        return MQTT_ERR_CONNECT;
    }
    // printf("[MQTT] TCP connected!\r\n");

    /* Send MQTT CONNECT packet */
    data.clientID.cstring = MQTT_CLIENT_ID;
    data.keepAliveInterval = MQTT_KEEPALIVE_SEC;
    data.cleansession = 1;
    data.MQTTVersion = 4;  /* MQTT 3.1.1 */

    len = MQTTSerialize_connect(mqtt_buf, sizeof(mqtt_buf), &data);
    // printf("[MQTT] CONNECT packet length: %d\r\n", len);
    
    if (len <= 0) {
        // printf("[MQTT] CONNECT serialize failed!\r\n");
        close(mqtt_socket);
        mqtt_socket = -1;
        return MQTT_ERR_CONNECT;
    }
    
    int sent = send(mqtt_socket, mqtt_buf, len, 0);
    // printf("[MQTT] CONNECT sent: %d bytes\r\n", sent);
    
    if (sent != len) {
        // printf("[MQTT] CONNECT send incomplete!\r\n");
        close(mqtt_socket);
        mqtt_socket = -1;
        return MQTT_ERR_CONNECT;
    }

    /* Wait for CONNACK */
    // printf("[MQTT] Waiting for CONNACK...\r\n");
    len = recv(mqtt_socket, mqtt_buf, sizeof(mqtt_buf), 0);
    // printf("[MQTT] CONNACK received: %d bytes\r\n", len);
    
    if (len <= 0) {
        // printf("[MQTT] CONNACK receive failed!\r\n");
        close(mqtt_socket);
        mqtt_socket = -1;
        return MQTT_ERR_CONNECT;
    }

    /* Verify CONNACK */
    unsigned char session_present, connack_rc;
    if (MQTTDeserialize_connack(&session_present, &connack_rc, mqtt_buf, len) != 1 || connack_rc != 0) {
        // printf("[MQTT] Connection rejected! rc=%d\r\n", connack_rc);
        close(mqtt_socket);
        mqtt_socket = -1;
        return MQTT_ERR_CONNECT;
    }

    // printf("[MQTT] *** BROKER CONNECTED SUCCESSFULLY ***\r\n");
    return MQTT_OK;
}

void gestlink_mqtt_disconnect(void) {
    if (mqtt_socket >= 0) {
        int len = MQTTSerialize_disconnect(mqtt_buf, sizeof(mqtt_buf));
        if (len > 0) {
            send(mqtt_socket, mqtt_buf, len, 0);
        }
        close(mqtt_socket);
        mqtt_socket = -1;
        // printf("[MQTT] Disconnected.\r\n");
    }
}

int gestlink_mqtt_is_connected(void) {
    return (mqtt_socket >= 0) ? 1 : 0;
}

int gestlink_mqtt_publish_gesture(uint8_t gesture_id) {
    char payload[100];
    MQTTString topic = MQTTString_initializer;
    int len;

    /* Check connection */
    if (mqtt_socket < 0) {
        return MQTT_ERR_NOT_CONNECTED;
    }

    /* Build JSON payload */
    snprintf(payload, sizeof(payload),
             "{\"gesture_id\":%d,\"gesture_name\":\"%s\"}",
             gesture_id, gestlink_mqtt_get_gesture_name(gesture_id));

    /* Serialize PUBLISH packet (QoS 0) */
    topic.cstring = MQTT_TOPIC;
    len = MQTTSerialize_publish(mqtt_buf, sizeof(mqtt_buf),
                                 0,    /* dup */
                                 0,    /* qos */
                                 0,    /* retained */
                                 0,    /* packet id */
                                 topic,
                                 (unsigned char*)payload,
                                 strlen(payload));

    if (len <= 0) {
        // printf("[MQTT] Publish serialize failed!\r\n");
        return MQTT_ERR_PUBLISH;
    }

    /* Send PUBLISH */
    if (send(mqtt_socket, mqtt_buf, len, 0) != len) {
        // printf("[MQTT] Publish send failed!\r\n");
        close(mqtt_socket);
        mqtt_socket = -1;
        return MQTT_ERR_PUBLISH;
    }

    // printf("[MQTT] Published: %s\r\n", payload);
    return MQTT_OK;
}

int gestlink_mqtt_send_keepalive(void) {
    int len;

    if (mqtt_socket < 0) {
        return MQTT_ERR_NOT_CONNECTED;
    }

    len = MQTTSerialize_pingreq(mqtt_buf, sizeof(mqtt_buf));
    if (len <= 0 || send(mqtt_socket, mqtt_buf, len, 0) != len) {
        // printf("[MQTT] Keepalive failed!\r\n");
        close(mqtt_socket);
        mqtt_socket = -1;
        return MQTT_ERR_PUBLISH;
    }

    return MQTT_OK;
}
