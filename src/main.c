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

struct DATABASE
{
    int8_t LED1;
    int8_t LED2;
    int8_t OUTPUT1;
    int8_t OUTPUT2;
    int8_t OUTPUT3;
    int8_t OUTPUT4;
    char* string_recieved;
    int64_t ThermTemp;
    uint32_t freq_UART;
    uint32_t freq_INPUTS;
    uint32_t freq_OUTPUTS;
    uint32_t freq_SENSOR;
}DATABASE;

struct DATABASE DB;

void UI()
{
    printf("\033[2J\033[H");                                // clear window code
    printf("\n String Recieved= %s", DB.string_recieved); 
    printf("\n"); 
    //printf("\n%s Movie A, 19H00 session, 9 EUR",arrow[0]); 
}

void main(void) 
{
    uartconfig();
    button_config();
    configure_threads();
    UI();
    return;
} 
