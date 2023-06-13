#include <unity.h>
#include "uart.h"




/*! Unity Function called at the beggining */
void setUp(void)											
{
	return;
}

/*! Unity Function called at the end 	  */
void tearDown(void)											
{
	return;
}

/*! Function using Unity used to test if the cmdProcessor returns EXIT_SUCESSFUL
*	when suposed to
*/
void test_uart_SENSOR_READING_SUCESSFUL(void)
{
	uint8_t rx_buf[] = "/s\r";      /* RX buffer, to store received data */
	TEST_ASSERT_EQUAL_INT(SENSOR_READING_SUCESSFUL, enter_routine(rx_buf));
}

void test_uart_INPUT_READING_SUCESSFUL(void)
{
	uint8_t rx_buf[] = "/b1\r";      /* RX buffer, to store received data */
	TEST_ASSERT_EQUAL_INT(INPUT_READING_SUCESSFUL, enter_routine(rx_buf));
}

void test_uart_OUTPUT_SET_SUCESSFUL(void)
{
	uint8_t rx_buf[] = "/o1_1\r";      /* RX buffer, to store received data */
	TEST_ASSERT_EQUAL_INT(OUTPUT_SET_SUCESSFUL, enter_routine(rx_buf));
}

void test_uart_FREQUENCY_CHANGE_SUCESSFUL(void)
{
	uint8_t rx_buf[] = "/fs2\r";      /* RX buffer, to store received data */
	TEST_ASSERT_EQUAL_INT(FREQUENCY_CHANGE_SUCESSFUL, enter_routine(rx_buf));
}

void test_uart_INVALID_COMMAND(void)
{
	uint8_t rx_buf[] = "/o1_x\r";      /* RX buffer, to store received data */
	TEST_ASSERT_EQUAL_INT(INVALID_COMMAND, enter_routine(rx_buf));
	
	strcpy(rx_buf, "/bx\r");	
	TEST_ASSERT_EQUAL_INT(INVALID_COMMAND, enter_routine(rx_buf));
	
	strcpy(rx_buf, "/avs\r");	
	TEST_ASSERT_EQUAL_INT(INVALID_COMMAND, enter_routine(rx_buf));
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_uart_SENSOR_READING_SUCESSFUL);	
	RUN_TEST(test_uart_INPUT_READING_SUCESSFUL);	
	RUN_TEST(test_uart_OUTPUT_SET_SUCESSFUL);	
	RUN_TEST(test_uart_FREQUENCY_CHANGE_SUCESSFUL);	
	RUN_TEST(test_uart_INVALID_COMMAND);	
		

	return UNITY_END();
}
