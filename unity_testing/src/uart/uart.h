/*!
 * \file:   uart.h
 * \author: Gonçalo Martins <goncalom23@ua.pt> and Filipe Silva <filipe.msilva@ua.pt>
 * 
 * \date May 29, 2023, 10:14 PM
 * \brief
 * Apropriate function prototypes, and some important definitions
 */

#include <stdio.h>                  /* for sprintf() */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

#ifndef uart_H
#define uart_H


#define EXIT_SUCESSFUL 0
#define SENSOR_READING_SUCESSFUL 1
#define INPUT_READING_SUCESSFUL 2
#define OUTPUT_SET_SUCESSFUL 3
#define FREQUENCY_CHANGE_SUCESSFUL 4
#define INVALID_COMMAND -1

#define RXBUF_SIZE 60                   /* RX buffer size */
#define TXBUF_SIZE 60                   /* TX buffer size */
#define RX_TIMEOUT 1000                 /* Inactivity period after the instant when last char was received that triggers an rx event (in us) */

struct DATABASE
{
    int8_t BUTTON1;
    int8_t BUTTON2;
    int8_t BUTTON3;
    int8_t BUTTON4;
    int8_t OUTPUT1;
    int8_t OUTPUT2;
    int8_t ThermTemp;
};

extern struct DATABASE DB;



extern uint8_t rx_buf[RXBUF_SIZE];      /* RX buffer, to store received data */
extern uint8_t rx_chars[RXBUF_SIZE];    /* chars actually received  */
extern volatile int uart_rxbuf_nchar;        /* Number of chars currrntly on the rx buffer */

int enter_routine(uint8_t rx_chars_aux[RXBUF_SIZE]);           
#endif
