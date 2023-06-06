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
#include "threads.h"

/* UART related variables */
const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);
uint8_t rx_buf[RXBUF_SIZE];      /* RX buffer, to store received data */
uint8_t rx_chars[RXBUF_SIZE];    /* chars actually received  */
volatile int uart_rxbuf_nchar=0;        /* Number of chars currrntly on the rx buffer */
uint8_t comand_state[RXBUF_SIZE]; 
uint8_t Led_1_new;
uint8_t Led_2_new;                    


/* Struct for UART configuration (if using default values is not needed) */
const struct uart_config uart_cfg = 
{
		.baudrate = 115200,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
		.data_bits = UART_CFG_DATA_BITS_8,
		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
};


void print_interface()
{
    printf("\033[2J\033[H");
    printf("\n UART frequency: %.0f", (int)1/(thread_UART_period* 0.001));
    printf("\n INPUTS frequency: %.0f", (int)1/(thread_INPUTS_period* 0.001));
    printf("\n OUTPUTS frequency: %.0f", (int)1/(thread_OUTPUTS_period* 0.001));
    printf("\n SENSOR frequency: %.0f", (int)1/(thread_SENSOR_period * 0.001));
    printf("\n");
    /*printf("\n Temp is: %d", DB.ThermTemp);
    printf("\n Button1 logic value: %u",DB.BUTTON1);
    printf("\n Button2 logic value: %u",DB.BUTTON2);
    printf("\n Button3 logic value: %u",DB.BUTTON3);
    printf("\n Button4 logic value: %u",DB.BUTTON4);*/
    printf("\n %s",comand_state);
    printf("\n");
    printf("\n Available commands:");
    printf("\n /fuxxx /fbxxx /fsxxx /foxxx /bx /ox_y /s");
    printf("\n String sent: %s",rx_chars);
}


void uartconfig()
{
/* Local vars */    
    int err=0; /* Generic error variable */

    /* Configure UART */
    err = uart_configure(uart_dev, &uart_cfg);
    if (err == -ENOSYS) { /* If invalid configuration */
        printk("uart_configure() error. Invalid configuration\n\r");
        return; 
    }
        
    /* Register callback */
    err = uart_callback_set(uart_dev, uart_cb, NULL);
    if (err) {
        printk("uart_callback_set() error. Error code:%d\n\r",err);
        return;
    }
		
    /* Enable data reception */
    err =  uart_rx_enable(uart_dev ,rx_buf,sizeof(rx_buf),RX_TIMEOUT);
    if (err) {
        printk("uart_rx_enable() error. Error code:%d\n\r",err);
        return;
    }
}


/* UART callback implementation */
/* Note that callback functions are executed in the scope of interrupt handlers. */
/* They run asynchronously after hardware/software interrupts and have a higher priority than all threads */
/* Should be kept as short and simple as possible. Heavier processing should be deferred to a task with suitable priority*/
void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
    int err;

    switch (evt->type) {
	
        case UART_TX_DONE:
		    printk("UART_TX_DONE event \n\r");
            break;

    	case UART_TX_ABORTED:
	    	printk("UART_TX_ABORTED event \n\r");
		    break;
		
	    case UART_RX_RDY:
		    //printk("UART_RX_RDY event \n\r");
            /* Just copy data to a buffer. Usually it is preferable to use e.g. a FIFO to communicate with a task that shall process the messages*/
            memcpy(&rx_chars[uart_rxbuf_nchar],&(rx_buf[evt->data.rx.offset]),evt->data.rx.len);
            if(rx_chars[uart_rxbuf_nchar] == '\r')
            {
                //printf("\nenter pressed");
                rx_chars[uart_rxbuf_nchar + 1] = '\0'; //recolocar terminador 
                enter_routine(rx_chars);
                rx_chars[0] = '\0';
                uart_rxbuf_nchar = 0;
            }
            else if(rx_chars[uart_rxbuf_nchar] == 127)
            {
                //printf("\nbackslash pressed");
                if(uart_rxbuf_nchar - 1 >= 0)
                {
                    rx_chars[uart_rxbuf_nchar - 1] = '\0';
                    uart_rxbuf_nchar = uart_rxbuf_nchar - 1;
                }
            }
            else
            {
                rx_chars[uart_rxbuf_nchar + 1] = '\0'; //recolocar terminador 
                uart_rxbuf_nchar++;   
            }
		    break;

	    case UART_RX_BUF_REQUEST:
		    printk("UART_RX_BUF_REQUEST event \n\r");
		    break;

	    case UART_RX_BUF_RELEASED:
		    printk("UART_RX_BUF_RELEASED event \n\r");
		    break;
		
	    case UART_RX_DISABLED: 
            /* When the RX_BUFF becomes full RX is is disabled automaticaly.  */
            /* It must be re-enabled manually for continuous reception */
            printk("UART_RX_DISABLED event \n\r");
		    err =  uart_rx_enable(uart_dev ,rx_buf,sizeof(rx_buf),RX_TIMEOUT);
            if (err) {
                printk("uart_rx_enable() error. Error code:%d\n\r",err);
                exit(FATAL_ERR);                
            }
		    break;

	    case UART_RX_STOPPED:
		    printk("UART_RX_STOPPED event \n\r");
		    break;
		
	    default:
            printk("UART: unknown event \n\r");
		    break;
    }
}

void enter_routine(uint8_t rx_chars_aux[RXBUF_SIZE])            // Executed when "Enter" is pressed on keyboard
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
            printf("\nFreq UART: %f",thread_UART_period);
        }
        if(rx_chars[2] == 'b')
        {
            thread_INPUTS_period = 1/(atoi(number_aux) * 0.001);
            printf("\nFreq INPUTS: %f",thread_INPUTS_period);
        }
        if(rx_chars[2] == 's')
        {
           thread_SENSOR_period = 1/(atoi(number_aux) * 0.001);
            printf("\nFreq_SENSOR: %f",thread_SENSOR_period);
        }
        if(rx_chars[2] == 'o')
        {
            thread_OUTPUTS_period = 1/(atoi(number_aux) * 0.001);
            printf("\nFreq_OUTPUTS: %f",thread_OUTPUTS_period);
        }
    }

    /* read button state
    *   /bx
    *   x - button to be read, available buttons 1-4.
    */ 
    else if(rx_chars[0] == '/' && rx_chars[1] == 'b' && (isdigit(rx_chars[2]) == 1 ))            
    {
        if(rx_chars[2] == '1')
        {
            uint8_t str_aux[RXBUF_SIZE];
            uint8_t str_message[] = "Button 1 state: "; 
            sprintf(str_aux,"%i",DB.BUTTON1);
            strcat(str_message,str_aux);
            strcpy(comand_state,str_message);
        }
        else if(rx_chars[2] == '2')
        {
            uint8_t str_aux[RXBUF_SIZE];
            uint8_t str_message[] = "Button 2 state: "; 
            sprintf(str_aux,"%i",DB.BUTTON2);
            strcat(str_message,str_aux);
            strcpy(comand_state,str_message);
        }
        else if(rx_chars[2] == '3')
        {
            uint8_t str_aux[RXBUF_SIZE];
            uint8_t str_message[] = "Button 3 state: "; 
            sprintf(str_aux,"%i",DB.BUTTON3);
            strcat(str_message,str_aux);
            strcpy(comand_state,str_message);
        }
        else if(rx_chars[2] == '4')
        {
            uint8_t str_aux[RXBUF_SIZE];
            uint8_t str_message[] = "Button 4 state: "; 
            sprintf(str_aux,"%i",DB.BUTTON4);
            strcat(str_message,str_aux);
            strcpy(comand_state,str_message);
        }
        else
        {
            printf("\nInvalid command");
            return;
        }
    }

    /* set output state 
    *   /ox_y
    *   x - output to be set, outputs available 1-2.
    *   y - state of the output
    */
    else if(rx_chars[0] == '/' && rx_chars[1] == 'o' && (isdigit(rx_chars[2]) == 1) && rx_chars[3] == '_' && (rx_chars[4] == '1' || rx_chars[4] == '0') )      
    {
        if(rx_chars[2] == '1')
        {
            k_sem_take(&sem_outputs,  K_FOREVER);   
            Led_1_new = rx_chars[4] - '0';
            k_sem_give(&sem_Led_1_update);
            k_sem_give(&sem_outputs);
            //printf("\nDB.OUTPUT1: %u",DB.OUTPUT1);
        }
        else if(rx_chars[2] == '2')
        {
            k_sem_take(&sem_outputs,  K_FOREVER);   
            Led_2_new = rx_chars[4] - '0';
            k_sem_give(&sem_Led_2_update);
            k_sem_give(&sem_outputs);
            //printf("\nDB.OUTPUT2: %u",DB.OUTPUT2);
        }
        else
        {
            printf("\nInvalid command");
            return;
        }

    }

    /* read sensor state */ 
    else if(rx_chars[0] == '/' && rx_chars[1] == 's')            
    {
        uint8_t str_aux[RXBUF_SIZE];
        uint8_t str_message[] = "Sensor temperature is: "; 
        k_sem_take(&sem_sensor,  K_FOREVER);
        sprintf(str_aux,"%d",DB.ThermTemp);
        k_sem_give(&sem_sensor); 
        strcat(str_message,str_aux);
        strcpy(comand_state,str_message);
    }

    else
    {
        printf("\nInvalid Command");
        return;
    }
}
