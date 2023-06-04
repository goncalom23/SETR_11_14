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


/* Struct for UART configuration (if using default values is not needed) */
const struct uart_config uart_cfg = 
{
		.baudrate = 115200,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
		.data_bits = UART_CFG_DATA_BITS_8,
		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
};


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
                printf("\nenter pressed");
                rx_chars[uart_rxbuf_nchar + 1] = '\0'; //recolocar terminador 
                enter_routine();
                rx_chars[0] = '\0';
                uart_rxbuf_nchar = 0;
            }
            else if(rx_chars[uart_rxbuf_nchar] == 127)
            {
                printf("\nbackslash pressed");
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

void enter_routine(void)            // Executed when "Enter" is pressed on keyboard
{
    /* SET FREQUENCIES COMMAND 
    * example for frequency set 20Hz to sensores, buttons and outputs
    * /fs20
    * /fb20
    * /fo20
    */
    if(rx_chars[0] == '/' && rx_chars[1] == 'f' && (rx_chars[2] == 'b' || rx_chars[2] == 's' || rx_chars[2] == 'o') )   
    {
        char *init = strchr(rx_chars, 'f');
        char *end = strchr(rx_chars, '\r');
        int len = end - init -2;
        char number_aux[len];
        strncpy(number_aux, init+2, len);
        number_aux[len+1] = '\0';
        if(rx_chars[2] == 'b')
        {
            DB.freq_INPUTS = atoi(number_aux);
            printf("\nDB.freq INPUTS: %i",DB.freq_INPUTS);
        }
        if(rx_chars[2] == 's')
        {
            DB.freq_SENSOR = atoi(number_aux);
            printf("\nDB.freq_SENSOR: %i",DB.freq_SENSOR);
        }
        if(rx_chars[2] == 'o')
        {
            DB.freq_OUTPUTS = atoi(number_aux);
            printf("\nDB.freq_OUTPUTS: %i",DB.freq_OUTPUTS);
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
            printf("\nButton 1 state: %i",DB.BUTTON1);
        }
        else if(rx_chars[2] == '2')
        {
            printf("\nButton 2 state: %i",DB.BUTTON2);

        }
        else if(rx_chars[2] == '3')
        {
            printf("\nButton 3 state: %i",DB.BUTTON3);

        }
        else if(rx_chars[2] == '4')
        {
            printf("\nButton 4 state: %i",DB.BUTTON4);

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
            DB.OUTPUT1 = (int)rx_chars[4];
            printf("\nDB.OUTPUT1: %u",DB.OUTPUT1);
        }
        else if(rx_chars[2] == '2')
        {
            DB.OUTPUT2 = (int)rx_chars[4];
            printf("\nDB.OUTPUT2: %u",DB.OUTPUT2);
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
    }

    else
    {
        printf("\nInvalid Command");
        return;
    }
}
