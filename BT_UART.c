#include "BT_UART.h"
#include "dac.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

// Declare MOTOR variables as gloabals
extern char left_motor[3];
extern char right_motor[3];

// Initialize GPIO pins
void GPIO_init() {
    // Configure peripherals for output LEDs
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    // Set GPIOB pins 0-3 to output
    GPIOB->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
    GPIOB->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0);
}

// Initialize Bluetooth UART
void BT_UART_init() {
    // Enable clock to UART Pins and GPIOA
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    // Configure GPIOA - turn on GPIOA pins
    // Using pins 9 and 10 because they are not connected to USB
    // PA9 is Tx, PA10 is Rx
    GPIOA->MODER &= ~(GPIO_MODER_MODE9 | GPIO_MODER_MODE10);
    GPIOA->MODER |= (GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1);
    // Configure pins into UART mode
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL9 | GPIO_AFRH_AFSEL10);
    GPIOA->AFR[1] |= ((7 << GPIO_AFRH_AFSEL9_Pos) | (7 << GPIO_AFRH_AFSEL10_Pos));
    // Configure UART
    // Baud rate / BRR = USARTDIV = 24MHz / 9600 : Should give a baud rate of 9600
    USART1->BRR = 2500;
    // Enable interrupt in vector table
    // Look in the vector table for UART Rx Interrupt, it should be in the printed packet
    NVIC->ISER[1] |= (1 << (USART1_IRQn & 0x1F));
    // Enable global interrupt
    __enable_irq();
    // Enable UART itself, enable Tx, Enable Rx, enable interrupts
    USART1->CR1 |= (USART_CR1_RE | USART_CR1_TE | USART_CR1_UE | USART_CR1_RXNEIE);
    // 1 start bit, 8 data bits
}

// Write a character to Bluetooth UART
void BT_UART_writeChar(uint8_t character) {
    // Wait until TX buffer is empty
    while (!(USART1->ISR & USART_ISR_TXE));
    // Send data
    USART1->TDR = character;
}

// Handle the UART interrupt
void USART1_IRQHandler(void) {
    // GPIOB->ODR |= GPIO_PIN_0;
    if (USART1->ISR & USART_ISR_RXNE) {
        uint8_t character;
        character = USART1->RDR - 48;
        if ((character & 1) && (character & 2)) {
            // Go straight, turn both motors on
            // GPIOB->ODR |= GPIO_PIN_0;
            // GPIOB->ODR |= GPIO_PIN_1;
            BT_UART_writeChar('T');
        } else if (character & 2) {
            // Drive right
            GPIOB->ODR &= ~(GPIO_PIN_2);
            DAC_set_voltage(right_motor);
            GPIOB->ODR |= GPIO_PIN_2;
        } else if (character & 1) {
            // Drive left
            left_motor[0] = '5';
            left_motor[1] = '0';
            left_motor[2] = '0';
            GPIOB->ODR &= ~(GPIO_PIN_1);
            DAC_set_voltage(left_motor);
            GPIOB->ODR |= GPIO_PIN_1;
        } else {
            // Turn off motors
            GPIOB->ODR |= GPIO_PIN_1;
            GPIOB->ODR |= GPIO_PIN_2;
        }
    }
}
