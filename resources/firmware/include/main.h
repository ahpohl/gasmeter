#ifndef MAIN_H
#define MAIN_H

#if __AVR_ATmega328P__

#define IRLED_PIN PD6              // IR LED pin
#define IRLED_DDR DDRD             // IR LED output register

#else
#error "Adapt pin definitions to your MCU"
#endif

#define UART_BAUD_RATE 19200        // baud rate

extern volatile uint16_t adc_value; // ADC raw value
extern volatile uint8_t timer_ready;

void ReadAdc(void);
void SendRawAdc(void);
void ReadGasMeter(void);

#endif
