/*
 * Paulo Pedreiras, 2023/02
 * Zephyr: Simple thread and digital IO example
 * 
 * Reads a button and sets a led according to the button state
 *
 * Base documentation:
 *  Zephyr kernel: 
 *  https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/kernel/services/index.html#kernel-services
 *  *
 *  HW info
 *      https://infocenter.nordicsemi.com/topic/struct_nrf52/struct/nrf52840.html
 *      Section: nRF52840 Product Specification -> Peripherals -> GPIO / GPIOTE
 * 
 *      Board specific HW info can be found in the nRF52840_DK_User_Guide_20201203. I/O pins available at pg 27
 * 
 * 
 */

#include <zephyr/kernel.h>          /* for kernel functions*/
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/timing/timing.h>   /* for timing services */
#include "threads.h"
#include "uart.h"
#include "button.h"

struct DATABASE DB;

#define LED0_NID DT_NODELABEL(led0)
#define LED1_NID DT_NODELABEL(led1)

struct gpio_dt_spec led0_dev = GPIO_DT_SPEC_GET(LED0_NID, gpios); /* GPIO device structure for LED0*/
struct gpio_dt_spec led1_dev = GPIO_DT_SPEC_GET(LED1_NID, gpios); /* GPIO device structure for LED1*/

void outputs_config()
{
    int ret = 0;
    if (!device_is_ready(led0_dev.port))  
	{
        printk("Fatal error: led0 device not ready!");
		return;
	}
    ret = gpio_pin_configure_dt(&led0_dev, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) 
    {
        printk("Failed to configure led0 \n\r");
	    return;
    }

	if (!device_is_ready(led1_dev.port))  
	{
        printk("Fatal error: led1 device not ready!");
		return;
	}
    ret = gpio_pin_configure_dt(&led1_dev, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) 
    {
        printk("Failed to configure led1 \n\r");
	    return;
    }

    gpio_pin_set_dt(&led0_dev,0);
    gpio_pin_set_dt(&led1_dev,0);
}


void main(void) 
{
    DB.BUTTON1 = 0;
    DB.BUTTON2 = 0;
    DB.BUTTON3 = 0;
    DB.BUTTON4 = 0;
    DB.OUTPUT1 = 0;
    DB.OUTPUT2 = 0;
    DB.ThermTemp = 0;
    outputs_config();
    uartconfig();
    button_config();
    configure_threads();
    
    return;
} 
