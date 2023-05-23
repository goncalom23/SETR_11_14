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

#define thread_A_prio 1       /* Thread scheduling priority */

#define thread_A_period 200         /* Therad periodicity (in ms)*/

K_THREAD_STACK_DEFINE(thread_A_stack, STACK_SIZE);  /* Create thread stack space */

struct k_thread thread_A_data;  /* Create variables for thread data */

k_tid_t thread_A_tid;       /* Create task IDs */

#define LED0_NID DT_NODELABEL(led0)         /* Refer to dts file */
#define BUT0_NID DT_NODELABEL(button0)

void configure_threads()
{
    thread_A_tid = k_thread_create(&thread_A_data, thread_A_stack,
    K_THREAD_STACK_SIZEOF(thread_A_stack), thread_A_code,
    NULL, NULL, NULL, thread_A_prio, 0, K_NO_WAIT);
}

/* Thread code implementation */
void thread_A_code(void *argA , void *argB, void *argC)
{
    /* Local vars */
    int64_t fin_time=0, release_time=0;     /* Timing variables to control task periodicity */
    const struct gpio_dt_spec led0_dev = GPIO_DT_SPEC_GET(LED0_NID, gpios); /* GPIO device structure for LED*/
    const struct gpio_dt_spec but0_dev = GPIO_DT_SPEC_GET(BUT0_NID, gpios); /* GPIO device structure for Button*/
        
    int ret=0;     /* Generic return value variable */
    
    /* Task init code */
    printk("Thread A init (periodic)\n");
    
    /* Check if Led and button devices are ready */
	if (!device_is_ready(led0_dev.port))  
	{
        printk("Fatal error: led0 device not ready!");
		return;
	}
    if (!device_is_ready(but0_dev.port))  
	{
        printk("Fatal error: but0 device not ready!");
		return;
	}

    /* Configure led0 and but0 IOs */
    ret = gpio_pin_configure_dt(&led0_dev, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        printk("Failed to configure led0 \n\r");
	    return;
    }
    ret = gpio_pin_configure_dt(&but0_dev, GPIO_INPUT | GPIO_PULL_UP);
    if (ret < 0) {
        printk("Failed to configure but0 \n\r");
	    return;
    }
           
    /* Compute next release instant */
    release_time = k_uptime_get() + thread_A_period;

    /* Thread loop */
    while(1) {        
        
        printk("Thread A activated\n\r");  
        
        if(gpio_pin_get_dt(&but0_dev)) {
            gpio_pin_set_dt(&led0_dev,1);
            printk("but0 active\n\r");
        } else {
            gpio_pin_set_dt(&led0_dev,0);
            printk("but0 not active\n\r");
        }
       
        /* Wait for next release instant */ 
        fin_time = k_uptime_get();
        if( fin_time < release_time) {
            k_msleep(release_time - fin_time);
            release_time += thread_A_period;

        }
    }

    /* Stop timing functions */
    timing_stop();
}

