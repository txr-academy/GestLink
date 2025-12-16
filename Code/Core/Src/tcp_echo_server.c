/*
 * tcp_echo_server.c
 *
 *  Created on: Dec 16, 2025
 *      Author: jyothish.tr
 */
#include "tcp_echo_server.h"
#include "lwip/tcp.h"
#include "lwip/debug.h"
#include "main.h" // For printf if needed

// Static Protocol Control Block
static struct tcp_pcb *echo_pcb;

// PROTOTYPES
static err_t tcp_echo_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_echo_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_echo_error(void *arg, err_t err);

/**
 * @brief  Initializes the TCP Echo Server
 * @param  None
 * @retval None
 */
void TCP_Echo_Init(void)
{
    // 1. Create a new TCP control block
    echo_pcb = tcp_new();

    if (echo_pcb != NULL)
    {
        err_t err;

        // 2. Bind the PCB to local IP (IP_ADDR_ANY) and port 7 (Standard Echo Port) or 5000
        err = tcp_bind(echo_pcb, IP_ADDR_ANY, 5000);

        if (err == ERR_OK)
        {
            // 3. Start listening for connections
            echo_pcb = tcp_listen(echo_pcb);

            // 4. Register the "Accept" callback function
            tcp_accept(echo_pcb, tcp_echo_accept);
            printf("TCP Server Started on Port 5000\n");
        }
        else
        {
            memp_free(MEMP_TCP_PCB, echo_pcb);
            printf("TCP Bind Failed\n");
        }
    }
}

/**
 * @brief  Callback called when a client connects to us
 */
static err_t tcp_echo_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    // Register the callback functions for THIS specific connection
    tcp_recv(newpcb, tcp_echo_recv);
    tcp_err(newpcb, tcp_echo_error); // Optional: Handle errors

    printf("Client Connected!\n");
    return ERR_OK;
}

/**
 * @brief  Callback called when data is received
 */
static err_t tcp_echo_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (p == NULL)
    {
        // Case: Client closed the connection
        printf("Client Disconnected\n");
        tcp_close(tpcb);
        return ERR_OK;
    }

    if (err != ERR_OK)
    {
        pbuf_free(p);
        return err;
    }

    // --- ECHO LOGIC ---
    // In a real application, you would parse data here.
    // For Echo: We just send the payload back.

    // 1. Acknowledge the data reception to update the TCP Window
    tcp_recved(tpcb, p->tot_len);

    // 2. Send the data back (Echo)
    // TCP_WRITE_FLAG_COPY: LwIP copies data to its own buffer (safe)
    err_t write_err = tcp_write(tpcb, p->payload, p->len, TCP_WRITE_FLAG_COPY);

    if (write_err == ERR_OK)
    {
        // Force sending the data immediately
        tcp_output(tpcb);
        printf("Echoed: %.*s\n", p->len, (char*)p->payload);
    }

    // 3. IMPORTANT: Free the packet buffer to avoid memory leaks
    pbuf_free(p);

    return ERR_OK;
}

static void tcp_echo_error(void *arg, err_t err)
{
    printf("TCP Error Occurred: %d\n", err);
}
