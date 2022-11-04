#ifndef MAIN_H
#define MAIN_H

#if __AVR_ATmega328P__

#define IRLED_PIN PD6              // IR LED pin
#define IRLED_DDR DDRD             // IR LED output register

//#define CLED_PIN PB1               // Counter LED pin
//#define CLED_DDR DDRB              // Counter LED output register
//#define CLED_PORT PORTB            // Counter LED port

#else
#error "Adapt pin definitions to your MCU"
#endif

#define UART_BAUD_RATE 19200        // baud rate

void ReadAdc(void);
void ReadGasMeter(void);

#endif
