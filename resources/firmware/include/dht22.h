#ifndef DHT22_H
#define DHT22_H

#if __AVR_ATmega328P__

#define DHT_PIN PB0              // DHT22 pin
#define DHT_DDR DDRB             // DHT22 output register
#define DHT_PORT PORTB           // DHT22 port

int GetTempHumidity(uint16_t *temperature, uint16_t *humidity);

#else
#error "Adapt pin definitions to your MCU"
#endif

#endif
