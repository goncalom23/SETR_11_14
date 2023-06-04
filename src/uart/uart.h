/*!
 * \file:   uart.h
 * \author: Gonçalo Martins <goncalom23@ua.pt> and Filipe Silva <filipe.msilva@ua.pt>
 * 
 * \date May 29, 2023, 10:14 PM
 * \brief
 * Apropriate function prototypes, and some important definitions
 */

#include <zephyr/kernel.h>          /* for k_msleep() */
#include <zephyr/device.h>          /* for device_is_ready() and device structure */
#include <zephyr/devicetree.h>	    /* for DT_NODELABEL() */
#include <zephyr/drivers/uart.h>    /* for ADC API*/
#include <zephyr/sys/printk.h>      /* for printk()*/
#include <stdio.h>                  /* for sprintf() */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#ifndef uart_H
#define uart_H

#define UART_NODE DT_NODELABEL(uart0)   /* UART0 node ID*/
#define MAIN_SLEEP_TIME_MS 1000 /* Time between main() activations */ 

#define FATAL_ERR -1 /* Fatal error return code, app terminates */

#define RXBUF_SIZE 60                   /* RX buffer size */
#define TXBUF_SIZE 60                   /* TX buffer size */
#define RX_TIMEOUT 1000                 /* Inactivity period after the instant when last char was received that triggers an rx event (in us) */

extern uint8_t rx_buf[RXBUF_SIZE];      /* RX buffer, to store received data */
extern uint8_t rx_chars[RXBUF_SIZE];    /* chars actually received  */
extern volatile int uart_rxbuf_nchar;        /* Number of chars currrntly on the rx buffer */

void uartconfig();
void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data);

void enter_routine(void);
#endif
