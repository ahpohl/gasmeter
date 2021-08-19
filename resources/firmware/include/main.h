#ifndef MAIN_H
#define MAIN_H

#if __AVR_ATmega328P__

#define IRLED_PIN PD6              // IR LED pin
#define IRLED_DDR DDRD             // IR LED output register

#else
#error "Adapt pin definitions to your MCU"
#endif

#define TIMER1_CLOCK 4             // Raw value resolution in Hz
#define UART_BAUD_RATE 9600        // baud rate

extern volatile uint8_t adc_ready;  // ADC ready flag
extern volatile uint16_t adc_value; // ADC raw value
extern volatile uint8_t tx_ready;   // transmit timer ready

// packet definition
typedef struct gasmeter {
  uint8_t volume_H;
  uint8_t volume_L;
  uint8_t temperature_H;
  uint8_t temperature_L;
  uint8_t humidity_H;
  uint8_t humidity_L;
} gasmeter_t;

// functions
extern void GetVolume(void);
extern void SendRawAdc(void);

#endif
