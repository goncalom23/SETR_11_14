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

#define I2C0_NODE DT_NODELABEL(tempsensor)

#define STACK_SIZE 1024     /* Size of stack area used by each thread (can be thread specific, if necessary)*/

#define thread_UART_prio 1       /* Thread scheduling priority */
#define thread_UART_period 1000         /* Therad periodicity (in ms)*/
K_THREAD_STACK_DEFINE(thread_UART_stack, STACK_SIZE);  /* Create thread stack space */
struct k_thread thread_UART_data;  /* Create variables for thread data */
k_tid_t thread_UART_tid;       /* Create task IDs */

#define thread_INPUTS_prio 1       /* Thread scheduling priority */
#define thread_INPUTS_period 200         /* Therad periodicity (in ms)*/
K_THREAD_STACK_DEFINE(thread_INPUTS_stack, STACK_SIZE);  /* Create thread stack space */
struct k_thread thread_INPUTS_data;  /* Create variables for thread data */
k_tid_t thread_INPUTS_tid;       /* Create task IDs */

#define thread_OUTPUTS_prio 1       /* Thread scheduling priority */
#define thread_OUTPUTS_period 200         /* Therad periodicity (in ms)*/
K_THREAD_STACK_DEFINE(thread_OUTPUTS_stack, STACK_SIZE);  /* Create thread stack space */
struct k_thread thread_OUTPUTS_data;  /* Create variables for thread data */
k_tid_t thread_OUTPUTS_tid;       /* Create task IDs */

#define thread_SENSOR_prio 1       /* Thread scheduling priority */
#define thread_SENSOR_period 200         /* Therad periodicity (in ms)*/
K_THREAD_STACK_DEFINE(thread_SENSOR_stack, STACK_SIZE);  /* Create thread stack space */
struct k_thread thread_SENSOR_data;  /* Create variables for thread data */
k_tid_t thread_SENSOR_tid;       /* Create task IDs */


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
}

/* Thread code implementation */
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
        if(rx_chars[uart_rxbuf_nchar-1] == '!')
        {
            int i = 0;
            char number_aux[8];
            while(rx_chars[i] != '#')
            {
                i++;
                if(rx_chars[i] == '!')
                {
                    printf("\nMissing initiator -> '#");
                    break;
                }
            }
            if(rx_chars[i+1] == 'B' || rx_chars[i+1] == 'S' || rx_chars[i+1] == 'O' )
            {
                char *init = strchr(rx_chars, '#');
                char *end = strchr(rx_chars, '!');
                int len = end - init-1;
                strncpy(number_aux, init+1, len);
                number_aux[len+1] = '\0';
                if(rx_chars[i+1] == 'B')
                {
                    DB.freq_INPUTS = atoi(number_aux);
                    printf("DB.freq INPUTS: %i",DB.freq_INPUTS);
                }
                if(rx_chars[i+1] == 'S')
                {
                    DB.freq_SENSOR = atoi(number_aux);
                    printf("DB.freq_SENSOR: %i",DB.freq_SENSOR);
                }
                if(rx_chars[i+1] == 'O')
                {
                    DB.freq_OUTPUTS = atoi(number_aux);
                    printf("DB.freq_OUTPUTS: %i",DB.freq_OUTPUTS);
                }
            else
            {
                printf("\nMissing Command Caracter");
                printf("\nString sent: %s",rx_chars);
                break;
            }
        }

            rx_chars[uart_rxbuf_nchar] = 0;
            uart_rxbuf_nchar = 0;
        }

        //printf("\033[2J\033[H");
        printf("\nAvailable commands:");
        printf("\n/fbxxx /fsxxx /foxxx /bx /ox_y /sx");
        printf("\nString sent: %s",rx_chars);

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
}

void thread_OUTPUTS_code()
{
}

void thread_SENSOR_code()
{
    /* Local vars */
    int64_t fin_time=0;
    int64_t release_time=0;     /* Timing variables to control task periodicity */

    uint8_t ret;

    if (!device_is_ready(dev_i2c.bus)) 
    {
	printf("I2C bus %s is not ready!\n\r",dev_i2c.bus->name);
	return;
    }
    uint8_t config[2] = {0x01,0x00};
    ret = i2c_write_dt(&dev_i2c, config, sizeof(config));
    if(ret != 0){
	printf("Failed to write to I2C device address %x at reg. %x n", dev_i2c.addr,config[0]);
    }
    /* Compute next release instant */
    release_time = k_uptime_get() + thread_UART_period;
    while(1){
    int8_t temp;

    ret = i2c_read_dt(&dev_i2c, &temp, sizeof(temp));

    printf("Temp is: %d", temp);
    DB.ThermTemp = temp;
    if(ret != 0){
	printf("Failed to read from I2C device address %x at Reg. %x n", dev_i2c.addr,config[0]);
    }

        fin_time = k_uptime_get();
        if( fin_time < release_time) 
        {
            k_msleep(release_time - fin_time);
            release_time += thread_UART_period;
        }
    }
}
