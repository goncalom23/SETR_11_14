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

#ifndef uart_H
#define uart_H

void uartconfig();
void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data);

#endif
