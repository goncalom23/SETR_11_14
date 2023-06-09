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
#include <stdio.h>
#include <string.h>

#ifndef threads_H
#define threads_H

struct DATABASE
{
    int8_t BUTTON1;
    int8_t BUTTON2;
    int8_t BUTTON3;
    int8_t BUTTON4;
    int8_t OUTPUT1;
    int8_t OUTPUT2;
    char* string_recieved;
    int8_t ThermTemp;
};

extern struct DATABASE DB;
extern struct k_sem sem_sensor;
extern struct k_sem sem_inputs;
extern struct k_sem sem_outputs;
extern struct k_sem sem_Led_1_update;
extern struct k_sem sem_Led_2_update;

extern struct gpio_dt_spec led0_dev;
extern struct gpio_dt_spec led1_dev;

extern float thread_UART_period;            /**< Thread periodicity (in ms)*/
extern float thread_INPUTS_period;          /**< Thread periodicity (in ms)*/
extern float thread_OUTPUTS_period;         /**< Thread periodicity (in ms)*/
extern float thread_SENSOR_period;          /**< Thread periodicity (in ms)*/


void configure_threads();
void thread_UART_code(void *argA, void *argB, void *argC); 
void thread_INPUTS_code();
void thread_OUTPUTS_code();
void thread_SENSOR_code();
void thread_Led_1_code();
void thread_Led_2_code();


#endif