#ifndef MAIN_H
#define MAIN_H

#if __AVR_ATmega328P__

#define OC0A_PIN PD6              // LED pin
#define OC0A_DDR DDRD             // Output register
#define OC1A_PIN PB1              // Timer 1 pin
#define OC1A_DDR DDRB             // Output register

#else
#error "Adapt pin definitions to your MCU"
#endif

#define TIMER0_CLOCK 24            // LED flashing frequency in Hz
#define TIMER1_CLOCK 4             // Raw value resolution in Hz

#define UART_BAUD_RATE 9600        // baud rate
#define UART_BUFFER_SIZE 32        // serial character buffer
#define UART_PACKET_SUCCESS 0x0000 // packet received or transmitted      
#define UART_CRC_ERROR 0x2000      // checksum error
#define UART_COMM_ERROR 0x4000     // communication error

#define CMD_COUNT_MODE 0x01        // gas counter mode
#define CMD_RAW_MODE 0x02          // send raw sensor values 
#define CMD_RX_COUNTS 0x04         // receive new gas counter
#define CMD_TX_COUNTS 0x08         // transmit current gas counter

// packet definition
typedef struct __attribute__ ((packed)) packet {
  uint32_t counter;
  int16_t high;
  int16_t low;
  int16_t max;
  int16_t min;
  int16_t value;
  uint16_t crc;
  uint8_t cmd;
  uint8_t status;
} packet_t;

extern volatile uint8_t aread;      // ADC ready flag
extern volatile uint16_t avalue;    // ADC raw value
extern volatile uint8_t txready;    // transmit timer ready
extern volatile packet_t rx_packet; // receive packet
extern volatile packet_t tx_packet; // transmit packet

extern void read_sensor(void);
extern unsigned int receive_packet(void);
extern void transmit_packet(void);

#endif // MAIN_H
