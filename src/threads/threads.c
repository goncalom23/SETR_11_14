/*!
 * \file:   threads.c
 * \author: Gonçalo Martins <goncalom23@ua.pt> and Filipe Silva <filipe.msilva@ua.pt> base on the
 * example code provided by Prof. Paulo Pedreiras
 * 
 * \date May 23, 2023, 10:44 AM
 * \brief
 * 
 */

#include "threads.h"

#define STACK_SIZE 1024     /* Size of stack area used by each thread (can be thread specific, if necessary)*/

#define thread_UART_prio 1       /* Thread scheduling priority */
#define thread_UART_period 200         /* Therad periodicity (in ms)*/
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
        printf("Thread UART\n\r");  
       
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
}
