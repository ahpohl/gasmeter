#include <string.h>
#include "uart.h"
#include "util.h"
#include "gasmeter.h"

uint8_t transmit_buffer[TX_SIZE] = {0};
uint8_t receive_buffer[RX_SIZE] = {0};

unsigned int ReceivePacket(void)
{
  unsigned int c;
  static int rx_len = 0;
  static unsigned char rx_buffer[UART_BUFFER_SIZE] = {0};

  // Get received character from ringbuffer
  // uart_getc() returns in the lower byte the received character and 
  // in the higher byte (bitmask) the last receive error
  c = uart_getc();
  if (c & UART_NO_DATA) {
    return UART_NO_DATA;
  }
  if (c & UART_FRAME_ERROR) {
    // Framing Error detected, i.e no stop bit detected
    uart_puts("UART framing error\n\r");
    return UART_FRAME_ERROR;
  }
  if (c & UART_OVERRUN_ERROR) {
    // Overrun, one or more received characters have been dropped
    uart_puts("UART overrun error\n\r");
    return UART_OVERRUN_ERROR;
  }
  if (c & UART_BUFFER_OVERFLOW) {
    // Overflow, We are not reading the receive buffer fast enough
    uart_puts("UART buffer overflow\n\r");
    return UART_BUFFER_OVERFLOW;
  }
  // save characters into buffer
  rx_buffer[rx_len] = (unsigned char)c;
  rx_len++;

  // check minimal packet size
  if (rx_len == RX_SIZE)
  {
    memcpy(receive_buffer, rx_buffer, RX_SIZE);
    // reset static rx buffer
    memset(rx_buffer, '\0', UART_BUFFER_SIZE);
    rx_len = 0;
    // check CRC inside packet match packet payload
    uint16_t crc_packet = (receive_buffer[RX_SIZE-1] << 8) | (receive_buffer[RX_SIZE-2] & 0xFF);
    uint16_t crc_payload = Crc16(receive_buffer, 0, RX_SIZE-2);
    if (crc_packet != crc_payload) {
      uart_puts("UART CRC error\n\r");
      return UART_CRC_ERROR;
    }
    SendBuffer(receive_buffer, RX_SIZE);
    return UART_PACKET_SUCCESS;
  }
  return UART_COMM_ERROR;
}
