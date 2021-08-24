#ifndef DHT22_H
#define DHT22_H

#if __AVR_ATmega328P__

#define DHT_PIN PB0              // DHT22 physical pin
#define DHT_DDR DDRB             // DHT22 data direction register
#define DHT_PORT PORTB           // DHT22 port register
#define DHT_INPUT PINB           // DHT22 pin input register

void GetTempHumidity(void);

#else
#error "Adapt pin definitions to your MCU"
#endif

#endif
