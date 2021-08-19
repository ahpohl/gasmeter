#ifndef GASMETER_H
#define GASMETER_H

#define UART_BUFFER_SIZE 8         // serial character buffer
#define UART_PACKET_SUCCESS 0x0000 // packet received 
#define UART_CRC_ERROR 0x2000      // checksum error
#define UART_BYTE_RECEIVED 0x4000  // one byte has been received

#define TX_SIZE 7                  // fixed send message
#define RX_SIZE 8                  // fixed receive message

#define EVERYTHING_IS_OK 0x00
#define COMMAND_NOT_IMPLEMENTED 0x01
#define VARIABLE_DOES_NOT_EXIST 0x02

extern volatile uint8_t packet_ready;  // packet ready flag
extern uint8_t receive_buffer[RX_SIZE];

// packet definition
typedef struct gasmeter {
  uint32_t volume;
  int16_t temperature;
  int16_t humidity;
} gasmeter_t;

// functions
void SendPacket(uint8_t state, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);
unsigned int ReceivePacket(void);
void ProcessPacket(void);

#endif
