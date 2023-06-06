/*!
 * \file:   threads.c
 * \author: Gonçalo Martins <goncalom23@ua.pt> and Filipe Silva <filipe.msilva@ua.pt> base on the
 * example code provided by Prof. Paulo Pedreiras
 * 
 * \date May 23, 2023, 10:44 AM
 * \brief
 * 
 */

#include <zephyr/drivers/i2c.h>
#include "threads.h"
#include "uart.h"
#include "button.h"

#define I2C0_NODE DT_NODELABEL(tempsensor)

#define STACK_SIZE 1024     /* Size of stack area used by each thread (can be thread specific, if necessary)*/

#define thread_UART_prio 1       /* Thread scheduling priority */
float thread_UART_period = 1000;
K_THREAD_STACK_DEFINE(thread_UART_stack, STACK_SIZE);  /* Create thread stack space */
struct k_thread thread_UART_data;  /* Create variables for thread data */
k_tid_t thread_UART_tid;       /* Create task IDs */

#define thread_INPUTS_prio 1       /* Thread scheduling priority */
float thread_INPUTS_period = 200;         /* Thread periodicity (in ms)*/
K_THREAD_STACK_DEFINE(thread_INPUTS_stack, STACK_SIZE);  /* Create thread stack space */
struct k_thread thread_INPUTS_data;  /* Create variables for thread data */
k_tid_t thread_INPUTS_tid;       /* Create task IDs */

#define thread_OUTPUTS_prio 1       /* Thread scheduling priority */
float thread_OUTPUTS_period = 200;         /* Thread periodicity (in ms)*/
K_THREAD_STACK_DEFINE(thread_OUTPUTS_stack, STACK_SIZE);  /* Create thread stack space */
struct k_thread thread_OUTPUTS_data;  /* Create variables for thread data */
k_tid_t thread_OUTPUTS_tid;       /* Create task IDs */

#define thread_SENSOR_prio 1       /* Thread scheduling priority */
float thread_SENSOR_period = 200;         /* Thread periodicity (in ms)*/
K_THREAD_STACK_DEFINE(thread_SENSOR_stack, STACK_SIZE);  /* Create thread stack space */
struct k_thread thread_SENSOR_data;  /* Create variables for thread data */
k_tid_t thread_SENSOR_tid;       /* Create task IDs */

#define thread_Led_1_prio 1       /* Thread scheduling priority */
K_THREAD_STACK_DEFINE(thread_Led_1_stack, STACK_SIZE);  /* Create thread stack space */
struct k_thread thread_Led_1_data;  /* Create variables for thread data */
k_tid_t thread_Led_1_tid;       /* Create task IDs */

#define thread_Led_2_prio 1       /* Thread scheduling priority */
K_THREAD_STACK_DEFINE(thread_Led_2_stack, STACK_SIZE);  /* Create thread stack space */
struct k_thread thread_Led2_data;  /* Create variables for thread data */
k_tid_t thread_Led_2_tid;       /* Create task IDs */

struct k_sem sem_sensor;
struct k_sem sem_outputs;
struct k_sem sem_Led_1_update;
struct k_sem sem_Led_2_update;

extern uint8_t Led_1_new;
extern uint8_t Led_2_new;

static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C0_NODE);

void configure_threads()
{
    thread_UART_tid = k_thread_create(&thread_UART_data, thread_UART_stack,
    K_THREAD_STACK_SIZEOF(thread_UART_stack), thread_UART_code,
    NULL, NULL, NULL, thread_UART_prio, 0, K_NO_WAIT);

    thread_INPUTS_tid = k_thread_create(&thread_INPUTS_data, thread_INPUTS_stack,
    K_THREAD_STACK_SIZEOF(thread_INPUTS_stack), thread_INPUTS_code,
    NULL, NULL, NULL, thread_INPUTS_prio, 0, K_NO_WAIT);

    thread_OUTPUTS_tid = k_thread_create(&thread_OUTPUTS_data, thread_OUTPUTS_stack,
    K_THREAD_STACK_SIZEOF(thread_OUTPUTS_stack), thread_OUTPUTS_code,
    NULL, NULL, NULL, thread_OUTPUTS_prio, 0, K_NO_WAIT);

    thread_SENSOR_tid = k_thread_create(&thread_SENSOR_data, thread_SENSOR_stack,
    K_THREAD_STACK_SIZEOF(thread_SENSOR_stack), thread_SENSOR_code,
    NULL, NULL, NULL, thread_SENSOR_prio, 0, K_NO_WAIT);

        thread_Led_1_tid = k_thread_create(&thread_Led_1_data, thread_Led_1_stack,
    K_THREAD_STACK_SIZEOF(thread_Led_1_stack), thread_Led_1_code,
    NULL, NULL, NULL, thread_Led_1_prio, 0, K_NO_WAIT);

        thread_Led_2_tid = k_thread_create(&thread_Led2_data, thread_Led_2_stack,
    K_THREAD_STACK_SIZEOF(thread_Led_2_stack), thread_Led_2_code,
    NULL, NULL, NULL, thread_SENSOR_prio, 0, K_NO_WAIT);

    k_sem_init(&sem_sensor, 1, 1);
    k_sem_init(&sem_outputs, 0, 1);
    k_sem_init(&sem_Led_1_update, 0, 1);
    k_sem_init(&sem_Led_2_update, 0, 1);
}

/* Thread code implementation */
void thread_Led_1_code(void *argA , void *argB, void *argC)
{
    /* Thread loop */
    while(1) 
    {   
        k_sem_take(&sem_Led_1_update,  K_FOREVER);
        DB.OUTPUT1 = Led_1_new;
    }
}

void thread_Led_2_code(void *argA , void *argB, void *argC)
{
    /* Thread loop */
    while(1) 
    {   
        k_sem_take(&sem_Led_2_update,  K_FOREVER);
        DB.OUTPUT2 = Led_2_new;
    }
}

void thread_UART_code(void *argA , void *argB, void *argC)
{
    /* Local vars */
    int64_t fin_time=0;
    int64_t release_time=0;     /* Timing variables to control task periodicity */

    /* Compute next release instant */
    release_time = k_uptime_get() + thread_UART_period;

    /* Thread loop */
    while(1) 
    {   
        print_interface();
        /* Wait for next release instant */ 
        fin_time = k_uptime_get();
        if( fin_time < release_time) 
        {
            k_msleep(release_time - fin_time);
            release_time += thread_UART_period;
        }
    }

    /* Stop timing functions */
    //timing_stop();
}

void thread_INPUTS_code()
{

    /* Local vars */
    int64_t fin_time=0;
    int64_t release_time=0;     /* Timing variables to control task periodicity */

    /* Compute next release instant */
    release_time = k_uptime_get() + thread_INPUTS_period;

    /* Thread loop */
    while(1) 
    {       
        DB.BUTTON1 = button_state[0];
        DB.BUTTON2 = button_state[1];
        DB.BUTTON3 = button_state[2];
        DB.BUTTON4 = button_state[3];

        /* Wait for next release instant */ 
        fin_time = k_uptime_get();
        if( fin_time < release_time) 
        {
            k_msleep(release_time - fin_time);
            release_time += thread_INPUTS_period;
        }
    }



}

void thread_OUTPUTS_code()
{
    /* Local vars */
    int64_t fin_time=0;
    int64_t release_time=0;     /* Timing variables to control task periodicity */

    /* Compute next release instant */
    release_time = k_uptime_get() + thread_OUTPUTS_period;
    /* Thread loop */
    while(1) 
    { 
        k_sem_take(&sem_outputs,  K_FOREVER);   
        if(DB.OUTPUT1 == 1)
        {
            gpio_pin_set_dt(&led0_dev,1);
        }
        else if(DB.OUTPUT1 == 0)
        {
            gpio_pin_set_dt(&led0_dev,0);
        } 

        if(DB.OUTPUT2 == 1)
        {
            gpio_pin_set_dt(&led1_dev,1);
        }
        else if(DB.OUTPUT2 == 0)
        {
            gpio_pin_set_dt(&led1_dev,0);
        } 
        k_sem_give(&sem_outputs);   


        fin_time = k_uptime_get();
        if( fin_time < release_time) 
        {
            k_msleep(release_time - fin_time);
            release_time += thread_OUTPUTS_period;
        }
    }
}

void thread_SENSOR_code()
{
    /* Local vars */
    int64_t fin_time=0;
    int64_t release_time=0;     /* Timing variables to control task periodicity */

    uint8_t ret;
    if (!device_is_ready(dev_i2c.bus)) 
    {
	printf("\nI2C bus %s is not ready!\n\r",dev_i2c.bus->name);
	return;
    }
    uint8_t config[2] = {0x00,0x00};
    ret = i2c_write_dt(&dev_i2c, config, sizeof(config));
    if(ret != 0)
    {
	printf("\nFailed to write to I2C device address %x at reg. %x n", dev_i2c.addr,config[0]);
    }
    /* Compute next release instant */
    release_time = k_uptime_get() + thread_UART_period;
    while(1)
    {
    int8_t temp;

    ret = i2c_read_dt(&dev_i2c, &temp, sizeof(temp));
    k_sem_take(&sem_sensor,  K_FOREVER);
    DB.ThermTemp = temp;
    k_sem_give(&sem_sensor);
    if(ret != 0)
    {
	printf("\nFailed to read from I2C device address %x at Reg. %x n", dev_i2c.addr,config[0]);
    }

        fin_time = k_uptime_get();
        if( fin_time < release_time) 
        {
            k_msleep(release_time - fin_time);
            release_time += thread_UART_period;
        }
    }
}
