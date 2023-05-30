/*!
 * \file:   button.c
 * \author: Gonçalo Martins <goncalom23@ua.pt> and Filipe Silva <filipe.msilva@ua.pt> base on the
 * example code provided by Prof. Paulo Pedreiras
 * 
 * \date May 15, 2023, 10:41 AM
 * \brief
 * 
 * SETR 22/23: Simple Digital I/O, using additional/external buttons
 * 
 * It shows the use of generic gpio functions. In these cases the pin functionality set by 
 * 		the DT (if any) is not used.
 *  
 * Base documentation:
 *        
 *      HW info:
 *          https://infocenter.nordicsemi.com/topic/struct_nrf52/struct/nrf52840.html
 *          Section: nRF52840 Product Specification -> Peripherals -> GPIO / GPIOTE
 * 
 *          Board specific HW info can be found in the nRF52840_DK_User_Guide_20201203. I/O pins available at pg 27
 *
 *          Note that Leds are active-low. I.e., they turn on when a logic zero is output at the corresponding port, and vice-versa.
 *          This behaviour can be reverted by adding the "GPIO_ACTIVE_LOW" flag - gpio_pin_config( ...., GPIO_OUTPUT_ACTIVE | GPIO_ACTIVE_LOW)
 * 			These flags can be obtained from the gpio device structure (e.g. led1.dt_flags) and directly used on the pin_config function
 *
 *
 *      SW API info:
 *          https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/reference/peripherals/gpio.html 
 * 
 * 
 * 	Notes:
 * 		Note that the GPIO and PWM must be enabled in prj.conf 
 * 		CONFIG_GPIO=y
 * 		 
 * 		Check prj.conf for additional contents that allows to see log messages 
 * 		and also printk() output
 * 
 *  --------------------------------------------------------------------------------------------------------------------------------
 * 
 *  Gonçalo Martins and Filipe Silva:
 *  Added array to track button states.
 * 	Modular and doxygen implementaiton;
 */

#include <zephyr/kernel.h>          /* for k_msleep()                             */
#include <zephyr/device.h>          /* for device_is_ready() and device structure */
#include <zephyr/devicetree.h>		/* for DT_NODELABEL()                         */
#include <zephyr/drivers/gpio.h>    /* for GPIO api                               */
#include <zephyr/sys/printk.h>      /* for printk()                               */
#include "button.h"

const uint8_t buttons_pins[] = {11,12,24,25,3,4,28,29};             /* vector with pins where buttons are connected */
static const struct device * gpio0_dev = DEVICE_DT_GET(GPIO0_NODE); /* Now get the device pointer for GPIO0         */

/* Define a variable of type static struct gpio_callback, which will latter be used to install the callback
*  It defines e.g. which pin triggers the callback and the address of the function */
static struct gpio_callback button_cb_data;

uint8_t button_state[8];                                            /* array to track all buttons state             */

/** \brief
 * Define a callback function. It is like an ISR (and runs in the cotext of an ISR) 
 * that is called when the button is pressed 
 *  \param[in] dev device or peripheral used in the interrupt rotine
 *  \param[in] cb represents a callback function that is executed when a specific GPIO event occurs
 *  \param[in] pins GPIO pins associated with the button                                                	 
 */ 
void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	int i=0;

	gpio_pin_toggle(gpio0_dev,LED1_PIN);                            /* Toggle led1 */

	/* Identify the button(s) that was(ere) hit*/
	for(i=0; i<sizeof(buttons_pins); i++)
	{		
		//button_state[i] = BIT(buttons_pins[i]) & pins;
		if(BIT(buttons_pins[i]) & pins) 
		{
			button_state[i] = 1;
		}
		else{
			button_state[i] = 0;
		}
	}   
}

/** \brief
 * functon that configures the GPIO pins as inputs and inicializes the interrupt routine                                             	 
 *  \return                                                     
 *  0: if success                 	 
 */ 
void button_config()
{
    int ret, i;
	uint32_t pinmask = 0;                                           /* Mask for setting the pins that shall generate interrupts */
	
	/* Welcome message */
	printk("Digital IO accessing IO pins not set via DT (external buttons in the case) \n\r");
	printk("Hit buttons 1-8 (1...4 internal, 5-8 external connected to A0...A3). Led toggles and button ID printed at console \n\r");

	/* Check if gpio0 device is ready */
	if (!device_is_ready(gpio0_dev)) 
    {
		printk("Error: gpio0 device is not ready\n");
		return;
	} 
    else 
    {
		printk("Success: gpio0 device is ready\n");
	}

	/* Configure the GPIO pins - LED1 for output and buttons 1-4 + IOPINS 2,4,28 and 29 for input
	 * Use internal pull-up to avoid the need for an external resistor (buttons) */
	ret = gpio_pin_configure(gpio0_dev,LED1_PIN, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) 
    {
		printk("Error: gpio_pin_configure failed for led1, error:%d\n\r", ret);
		return;
	}

	for(i=0; i<sizeof(buttons_pins); i++) 
    {
		ret = gpio_pin_configure(gpio0_dev, buttons_pins[i], GPIO_INPUT | GPIO_PULL_UP);
		if (ret < 0) 
        {
			printk("Error: gpio_pin_configure failed for button %d/pin %d, error:%d\n\r", i+1,buttons_pins[i], ret);
			return;
		} 
        else 
        {
			printk("Success: gpio_pin_configure for button %d/pin %d\n\r", i+1,buttons_pins[i]);
		}
	}

	/* Configure the interrupt on the button's pin */
	for(i=0; i<sizeof(buttons_pins); i++) 
    {
		ret = gpio_pin_interrupt_configure(gpio0_dev, buttons_pins[i], GPIO_INT_EDGE_TO_ACTIVE );
		if (ret < 0) 
        {
			printk("Error: gpio_pin_interrupt_configure failed for button %d / pin %d, error:%d", i+1, buttons_pins[i], ret);
			return;
		}
	}

	/* HW init done!*/
	printk("All devices initialized sucesfully!\n\r");

	/* Initialize the static struct gpio_callback variable   */
	pinmask=0;
	for(i=0; i<sizeof(buttons_pins); i++) 
    {
		pinmask |= BIT(buttons_pins[i]);
	}
    gpio_init_callback(&button_cb_data, button_pressed, pinmask); 	
	
	/* Add the callback function by calling gpio_add_callback()   */
	gpio_add_callback(gpio0_dev, &button_cb_data);
}
