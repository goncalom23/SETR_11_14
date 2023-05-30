/*!
 * \file:   button.h
 * \author: Gonçalo Martins <goncalom23@ua.pt> and Filipe Silva <filipe.msilva@ua.pt>
 * 
 * \date May 15, 2023, 10:41 AM
 * \brief
 * Apropriate function prototypes, and some important definitions
 */

#include <zephyr/kernel.h>              /*!< for k_msleep() */
#include <zephyr/device.h>              /*!< for device_is_ready() and device structure */
#include <zephyr/devicetree.h>		    /*!< for DT_NODELABEL() */
#include <zephyr/drivers/gpio.h>        /*!< for GPIO api*/
#include <zephyr/sys/printk.h>          /*!< for printk()*/

#ifndef button_H
#define button_H

#define SLEEP_TIME_MS   60*1000         /*!< Use a "big" sleep time to reduce CPU load (button detection int activated, not polled) */

/* Set the pins used for LED and buttons                                        */
/* LED 1 and buttons 1-4 are the ones on board                                  */
/* buttons 5-8 are connected to pins labeled A0 ... A3 (gpio0 pins 3,4,28,29)   */
#define LED1_PIN 13

#define GPIO0_NODE DT_NODELABEL(gpio0)  /*!< Get node ID for GPIO0, which has leds and buttons */ 

extern uint8_t button_state[8];         /* array to track all buttons state */

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
void button_config();


#endif