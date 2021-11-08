#ifndef MAIN_H
#define MAIN_H

#if __AVR_ATmega328P__

#define IRLED_PIN PD6              // IR LED pin
#define IRLED_DDR DDRD             // IR LED output register

#else
#error "Adapt pin definitions to your MCU"
#endif

#define UART_BAUD_RATE 19200        // baud rate

void ReadAdc(void);
void ReadGasMeter(void);

extern uint32_t VolumeEepromAddr;

#endif
