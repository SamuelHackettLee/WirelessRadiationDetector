#ifndef BT_UART_H_
#define BT_UART_H_
#include "main.h"
#define ESC 0x1B
void GPIO_init();
void BT_UART_init();
void BT_UART_writeChar(uint8_t character);
#endif
