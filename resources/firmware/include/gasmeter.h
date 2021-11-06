#ifndef GASMETER_H
#define GASMETER_H

#define UART_BUFFER_SIZE 8         // serial character buffer
#define TX_SIZE 7                  // fixed send message
#define RX_SIZE 8                  // fixed receive message

// gasmeter error codes (continue uart.h error codes)
#define UART_CRC_ERROR 0x2000
#define COMMAND_NOT_IMPLEMENTED 0x4000
#define VARIABLE_DOES_NOT_EXIST 0x8000

// packet definition
typedef struct gasmeter {
  int32_t volume;
  int32_t temperature;
  int32_t humidity;
  int16_t level_low;
  int16_t level_high;
} gasmeter_t;

// global variables
extern gasmeter_t volatile gasmeter;
extern uint8_t volatile packet_ready;
extern uint8_t receive_buffer[RX_SIZE];
extern uint8_t error_code;

// functions
void SendPacket(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);
void ReceivePacket(void);
void ProcessPacket(void);
void ReadGasMeter(void);

#endif
