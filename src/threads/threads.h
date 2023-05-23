/*!
 * \file:   threads.h
 * \author: Gonçalo Martins <goncalom23@ua.pt> and Filipe Silva <filipe.msilva@ua.pt>
 * 
 * \date May 23, 2023, 10:44 AM
 * \brief
 * Apropriate function prototypes, and some important definitions
 */

#include <zephyr/kernel.h>          /* for kernel functions*/
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/timing/timing.h>   /* for timing services */

#ifndef threads_H
#define threads_H

void configure_threads();
void thread_A_code(void *argA, void *argB, void *argC); /* Thread code prototypes */


#endif