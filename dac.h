#ifndef DAC_H
#define DAC_H
/*
NOTE:
    --  DAC_sel equation comes from:
        Vout = ( (Vref * Dn) * G ) / 2^12
            where:  2^12 is number of DAC selections (NUM_VSEL)
                    G = 1 for Low Gain mode. See note below for why only 1 used
        Multiplying before dividing to prevent usage of floats

    --  Since input is limited in DAC_set_voltage to 3 digits, we can
        simplify code by only worrying about High gain for the max voltage
        of 3.30V

    --  Don't need to allow for other voltages. We don't want to use
        higher voltages to prevent damage to STM pins.

    --  Datasheet says max clock supported by DAC is 20MHz. This is well
        above default STM clock of 4MHz, so we can use highest baudrate
        possible on board.
*/

/* Default DAC setup:   SPI 2
    - PB12: SPI2_NSS
    - PB13: SPI2_SCK
    - PB15: SPI2_MOSI (PICO)
*/

/* DAC config bits 15 - 12
    - 15: 0 (Write to DAC reg)
    - 14: 0 (Unbuffered Vref input)
    - 13: 0 for High Gain, 1 for Low Gain
    - 12: 1 (Vout enabled)
*/

/* SPI config fields (CR1, CR2 registers):
    - CPOL  : 0 (CLK idles low)
    - CPHA  : 0 (1st edge for data "rising edge")
    - MSTR  : 1 (Master config)
    - BR    : 000 (PCLK / 2, see header)
    - LSB   : 0 (MSB first)
    - #bits : 1111 (16 bits)
    - SSOE  : 1 (SS output enabled, can't run multimasters)
    - NSSP  : 1 (Hardward controled CS, active low)
*/

#include "stm32l4xx_hal.h"


#define VREF            5000    // Reference voltage used for DAC in millivolts
#define NUM_VSEL        4096    // Number of possible selections for DAC
#define MAX_V_LOW_GAIN  3299    // Max voltage output w/ Low Gain in millivolts
#define CONFIG_Msk    0xF000    // Mask for the top 4 config bits of DAC
#define LOW_G_CONFIG  0x3000    // Bits for Low Gain output

void DAC_init_default();
uint8_t DAC_set_voltage(char volt_str[3]);
uint16_t get_DAC_sel(uint16_t volt_mv);
void DAC_write(uint16_t val);


#endif
