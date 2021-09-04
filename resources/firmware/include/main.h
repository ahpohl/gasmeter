#ifndef MAIN_H
#define MAIN_H

#if __AVR_ATmega328P__

#define IRLED_PIN PD6              // IR LED pin
#define IRLED_DDR DDRD             // IR LED output register

#else
#error "Adapt pin definitions to your MCU"
#endif

#define UART_BAUD_RATE 9600        // baud rate

extern volatile uint8_t adc_ready;  // ADC ready flag
extern volatile uint16_t adc_value; // ADC raw value

#endif
