#include <string.h>
#include "uart.h"
#include "util.h"
#include "gasmeter.h"

uint8_t rx_packet[RX_SIZE] = {0};
gasmeter_t gasmeter = {};
uint8_t error_code;

void ReceivePacket(void)
{
  unsigned int c;
  static int bytes_received = 0;
  static unsigned char rx_buffer[UART_BUFFER_SIZE] = {0};
  // Get received character from ringbuffer
  // uart_getc() returns in the lower byte the received character and 
  // in the higher byte (bitmask) the last receive error
  c = uart_getc();
  if (c & UART_NO_DATA) {
    //error_code = (uint8_t) (UART_NO_DATA >> 8);
    return;
  }
  if (c & UART_FRAME_ERROR) {
    // Framing Error detected, i.e no stop bit detected
    error_code = (uint8_t) (UART_FRAME_ERROR >> 8);
    return;
  }
  if (c & UART_OVERRUN_ERROR) {
    // Overrun, one or more received characters have been dropped
    error_code = (uint8_t) (UART_OVERRUN_ERROR >> 8);
    return;
  }
  if (c & UART_BUFFER_OVERFLOW) {
    // Overflow, We are not reading the receive buffer fast enough
    error_code = (uint8_t) (UART_BUFFER_OVERFLOW >> 8);
    return;
  }
  // save characters into buffer
  if (bytes_received == 0)
  {
    memset(rx_buffer, 0, UART_BUFFER_SIZE);
  }
  rx_buffer[bytes_received] = (unsigned char)c;
  bytes_received++;

  // process receive buffer
  if (bytes_received == RX_SIZE)
  {
    bytes_received = 0;
    // check CRC inside packet match packet payload
    uint16_t crc_packet = (rx_buffer[RX_SIZE-1] << 8) | (rx_buffer[RX_SIZE-2] & 0xFF);
    uint16_t crc_payload = Crc16Ccitt(rx_buffer, RX_SIZE-2);
    if (crc_packet != crc_payload) {
      error_code = (uint8_t) (UART_CRC_ERROR >> 8);
      return;
    }
    memcpy(rx_packet, rx_buffer, RX_SIZE);
  }
}

void SendPacket(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
{
  uint8_t tx_buffer[TX_SIZE] = {0};

  tx_buffer[0] = error_code;
  tx_buffer[1] = b1;
  tx_buffer[2] = b2;
  tx_buffer[3] = b3;
  tx_buffer[4] = b4;

  uint16_t crc_payload = Crc16Ccitt(tx_buffer, TX_SIZE-2);
  tx_buffer[5] = (uint8_t) ((crc_payload >> 8) & 0xFF);
  tx_buffer[6] = (uint8_t) crc_payload;
  
  for (int i = 0; i < TX_SIZE; i++)
  {
    uart_putc(tx_buffer[i]);
  }
}

void ProcessPacket(void)
{
  if (error_code)
  {
    SendPacket(0, 0, 0, 0);
    error_code = 0;
    return;
  }
  uint8_t b[4] = {0};

  switch (rx_packet[0])
  {
  case 1: // pre-set volume
    uint32_t volume;
    memcpy(&volume, rx_packet+2, sizeof(volume));
    if (volume > gasmeter.volume)
    {
      gasmeter.volume = volume;
    }
    memcpy(&b, &gasmeter.volume, sizeof(b));
    break;
  case 2: // measure request to DSP
    switch (rx_packet[1])
    {
    case 1: // meter reading
      memcpy(&b, &gasmeter.volume, sizeof(b));
      break;
    case 2: // temperature
      b[2] = (uint8_t) ((gasmeter.temperature >> 8) & 0xFF);
      b[3] = (uint8_t) gasmeter.temperature;
      break;
    case 3: // humidity
      b[2] = (uint8_t) ((gasmeter.humidity >> 8) & 0xFF);
      b[3] = (uint8_t) gasmeter.humidity;
      break;
    default:
      error_code = (uint8_t) (VARIABLE_DOES_NOT_EXIST >> 8);
      break;
    }
    break;
  default:
    error_code = (uint8_t) (COMMAND_NOT_IMPLEMENTED >> 8);
    break;
  }
  SendPacket(b[0], b[1], b[2], b[3]);
  error_code = 0;
}
