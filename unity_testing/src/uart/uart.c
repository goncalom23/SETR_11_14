/*!
 * \file:   uart.c
 * \author: Gonçalo Martins <goncalom23@ua.pt> and Filipe Silva <filipe.msilva@ua.pt> base on the
 * example code provided by Prof. Paulo Pedreiras
 * 
 * \date May 29, 2023, 10:15 PM
 * \brief
 * 
 */

#include "uart.h"

float thread_UART_period = 1000;
float thread_INPUTS_period = 200;         /* Thread periodicity (in ms)*/
float thread_OUTPUTS_period = 200;         /* Thread periodicity (in ms)*/
float thread_SENSOR_period = 200;         /* Thread periodicity (in ms)*/

struct DATABASE DB;

/* UART related variables */
uint8_t rx_buf[RXBUF_SIZE];      /* RX buffer, to store received data */
uint8_t rx_chars[RXBUF_SIZE];    /* chars actually received  */
volatile int uart_rxbuf_nchar=0;        /* Number of chars currrntly on the rx buffer */
uint8_t comand_state[RXBUF_SIZE];                     

int enter_routine(uint8_t rx_chars_aux[RXBUF_SIZE])           
{
    strcpy(rx_chars,rx_chars_aux);
    /* SET FREQUENCIES COMMAND 
    * example for frequency set 20Hz to sensores, buttons and outputs
    * /fs20
    * /fb20
    * /fo20
    */
    if(rx_chars[0] == '/' && rx_chars[1] == 'f' && (rx_chars[2] == 'b' || rx_chars[2] == 's' || rx_chars[2] == 'o' || rx_chars[2] == 'u') )   
    {
        char *init = strchr(rx_chars, 'f');
        char *end = strchr(rx_chars, '\r');
        int len = end - init -2;
        char number_aux[len];
        strncpy(number_aux, init+2, len);
        number_aux[len+1] = '\0';
        if(rx_chars[2] == 'u')
        {
            thread_UART_period = 1/(atoi(number_aux) * 0.001);
            /*printf("\nFreq UART: %f",thread_UART_period);*/
            return FREQUENCY_CHANGE_SUCESSFUL;
        }
        if(rx_chars[2] == 'b')
        {
            thread_INPUTS_period = 1/(atoi(number_aux) * 0.001);
            /*printf("\nFreq INPUTS: %f",thread_INPUTS_period);*/
            return FREQUENCY_CHANGE_SUCESSFUL;   
        }
        if(rx_chars[2] == 's')
        {
           thread_SENSOR_period = 1/(atoi(number_aux) * 0.001);
            /*printf("\nFreq_SENSOR: %f",thread_SENSOR_period);*/
            return FREQUENCY_CHANGE_SUCESSFUL;
        }
        if(rx_chars[2] == 'o')
        {
            thread_OUTPUTS_period = 1/(atoi(number_aux) * 0.001);
            /*printf("\nFreq_OUTPUTS: %f",thread_OUTPUTS_period);*/
            return FREQUENCY_CHANGE_SUCESSFUL;
        }
    }

    /* read button state
    *   /bx
    *   x - button to be read, available buttons 1-4.
    */ 
    else if(rx_chars[0] == '/' && rx_chars[1] == 'b' && (isdigit(rx_chars[2]) != 0 ))            
    {
        if(rx_chars[2] == '1')
        {
            uint8_t str_aux[RXBUF_SIZE];
            uint8_t str_message[] = "Button 1 state: "; 
            sprintf(str_aux,"%i",DB.BUTTON1);
            strcat(str_message,str_aux);
            strcpy(comand_state,str_message);
            return INPUT_READING_SUCESSFUL;
        }
        else if(rx_chars[2] == '2')
        {
            uint8_t str_aux[RXBUF_SIZE];
            uint8_t str_message[] = "Button 2 state: "; 
            sprintf(str_aux,"%i",DB.BUTTON2);
            strcat(str_message,str_aux);
            strcpy(comand_state,str_message);
            return INPUT_READING_SUCESSFUL;
        }
        else if(rx_chars[2] == '3')
        {
            uint8_t str_aux[RXBUF_SIZE];
            uint8_t str_message[] = "Button 3 state: "; 
            sprintf(str_aux,"%i",DB.BUTTON3);
            strcat(str_message,str_aux);
            strcpy(comand_state,str_message);
            return INPUT_READING_SUCESSFUL;
        }
        else if(rx_chars[2] == '4')
        {
            uint8_t str_aux[RXBUF_SIZE];
            uint8_t str_message[] = "Button 4 state: "; 
            sprintf(str_aux,"%i",DB.BUTTON4);
            strcat(str_message,str_aux);
            strcpy(comand_state,str_message);
            return INPUT_READING_SUCESSFUL;
        }
        else
        {
            printf("\nInvalid command");
            return INVALID_COMMAND;
        }
    }

    /* set output state 
    *   /ox_y
    *   x - output to be set, outputs available 1-2.
    *   y - state of the output
    */
    else if(rx_chars[0] == '/' && rx_chars[1] == 'o' && (isdigit(rx_chars[2]) != 0) && rx_chars[3] == '_' && (rx_chars[4] == '1' || rx_chars[4] == '0') )      
    {
        if(rx_chars[2] == '1')
        {
            DB.OUTPUT1 = rx_chars[4] - '0';
            return OUTPUT_SET_SUCESSFUL;
        }
        else if(rx_chars[2] == '2')
        {
            DB.OUTPUT2 = rx_chars[4] - '0';
            return OUTPUT_SET_SUCESSFUL;
        }
        else
        {
            printf("\nInvalid command");
            return INVALID_COMMAND;
        }

    }

    /* read sensor state */ 
    else if(rx_chars[0] == '/' && rx_chars[1] == 's')            
    {
        uint8_t str_aux[RXBUF_SIZE];
        uint8_t str_message[] = "Sensor temperature is: "; 
        sprintf(str_aux,"%d",DB.ThermTemp);
        strcat(str_message,str_aux);
        strcpy(comand_state,str_message);
        return SENSOR_READING_SUCESSFUL;
    }

    else
    {
        /*printf("\nInvalid Command");*/
        return INVALID_COMMAND;
    }
}
