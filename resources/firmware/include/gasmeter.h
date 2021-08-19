#ifndef GASMETER_H
#define GASMETER_H

#define UART_BUFFER_SIZE 32        // serial character buffer
#define UART_PACKET_SUCCESS 0x0000 // packet received or transmitted      
#define UART_CRC_ERROR 0x2000      // checksum error
#define UART_COMM_ERROR 0x4000     // communication error

#define TX_SIZE 7                  // fixed send message
#define RX_SIZE 8                  // fixed receive message

extern uint8_t receive_buffer[RX_SIZE];

// functions
extern void SendPacket(uint8_t state, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);
extern unsigned int ReceivePacket(void);

#endif
