#include "msg.h"


void send_float(float value) {
	volatile float data = value;
	uart_send_data(&data, sizeof(data));
}

void send_int(int value) {
	volatile int data = value;
	uart_send_data(&data, sizeof(data));
}
