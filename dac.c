#include "dac.h"

void DAC_init_default() {
    // Enable clock to Port B and SPI 2
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_SPI2EN;

    // Set Pin mode for B12, B13, B15 to Alt Function (10)
    GPIOB->MODER &= ~(GPIO_MODER_MODE12 | GPIO_MODER_MODE13 | GPIO_MODER_MODE15);
    GPIOB->MODER |= (GPIO_MODER_MODE12_1 | GPIO_MODER_MODE13_1 | GPIO_MODER_MODE15_1);

    // Set pins to SPI Alt Function (5)
    GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL12 | GPIO_AFRH_AFSEL13 | GPIO_AFRH_AFSEL15);
    GPIOB->AFR[1] |= (5 << GPIO_AFRH_AFSEL12_Pos) | (5 << GPIO_AFRH_AFSEL13_Pos) | (5 << GPIO_AFRH_AFSEL15_Pos);

    // Configure SPI CR registers. See header for info
    // Set CPOL, CPHA, LSB, BR to 0
    SPI2->CR1 &= ~(SPI_CR1_CPHA | SPI_CR1_CPOL | SPI_CR1_LSBFIRST | SPI_CR1_BR);

    // Set Master config to 1
    SPI2->CR1 |= SPI_CR1_MSTR;

    // Set bits, SSOE, NSSP to all 1
    SPI2->CR2 |= (SPI_CR2_DS | SPI_CR2_SSOE); // | SPI_CR2_NSSP );   //Want to handle multi CS

    // Enable SPI2
    SPI2->CR1 |= SPI_CR1_SPE;
}

void DAC_write(uint16_t val) {
    SPI2->DR = val;
    while (!(SPI2->SR & SPI_SR_TXE));
}

uint16_t get_DAC_sel(uint16_t volt_mv) {
    // Check for max output
    if (volt_mv >= VREF)
        return 0x1800;

    // Calculate Low Gain selection
    uint32_t DAC_sel = (volt_mv * NUM_VSEL) / VREF;

    // Insert Config bits. See header for Config bit info
    DAC_sel &= ~(CONFIG_Msk);
    DAC_sel |= LOW_G_CONFIG;

    return (uint16_t)DAC_sel;
}

uint8_t DAC_set_voltage(char volt_str[3]) {
    // Validate Voltage selection
    for (uint8_t i = 0; i < 3; i++) {
        if (volt_str[i] < '0' || volt_str[i] > '9')
            return 1;
    }

    // Convert volt str to millivolts
    uint16_t out_volt_mv = ((volt_str[0] - 48) * 1000) + ((volt_str[1] - 48) * 100) + ((volt_str[2] - 48) * 10);

    // Get DAC selection value
    uint16_t DAC_sel = get_DAC_sel(out_volt_mv);

    // Control DAC to set Voltage Output
    DAC_write(DAC_sel);

    return 0;
}
