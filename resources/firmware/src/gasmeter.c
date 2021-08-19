#include <string.h>
#include "uart.h"
#include "util.h"
#include "gasmeter.h"

volatile uint8_t packet_ready = 0;
uint8_t rx_packet[RX_SIZE] = {0};
gasmeter_t gasmeter = {};

unsigned int ReceivePacket(void)
{
  unsigned int c;
  static int bytes_received = 0;
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
    return UART_FRAME_ERROR;
  }
  if (c & UART_OVERRUN_ERROR) {
    // Overrun, one or more received characters have been dropped
    return UART_OVERRUN_ERROR;
  }
  if (c & UART_BUFFER_OVERFLOW) {
    // Overflow, We are not reading the receive buffer fast enough
    return UART_BUFFER_OVERFLOW;
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
    uint16_t crc_payload = Crc16(rx_buffer, 0, RX_SIZE-2);
    if (crc_packet != crc_payload) {
      return UART_CRC_ERROR;
    }
    memcpy(rx_packet, rx_buffer, RX_SIZE);
    packet_ready = 1;
    return UART_PACKET_SUCCESS;
  }
  return UART_BYTE_RECEIVED;
}

void SendPacket(uint8_t state, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
{
  uint8_t tx_buffer[TX_SIZE] = {0};

  tx_buffer[0] = state;
  tx_buffer[1] = b1;
  tx_buffer[2] = b2;
  tx_buffer[3] = b3;
  tx_buffer[4] = b4;

  uint16_t crc_payload = Crc16(tx_buffer, 0, TX_SIZE-2);
  tx_buffer[5] = crc_payload >> 8;
  tx_buffer[6] = crc_payload & 0xFF;
  SendBuffer(tx_buffer, TX_SIZE);
}

void ProcessPacket(void)
{
  if (!(packet_ready))
  {
    return;
  }
  uint8_t status = EVERYTHING_IS_OK;
  uint8_t b1 = 0, b2 = 0, b3 = 0, b4 = 0;

  switch (rx_packet[0])
  {
  case 1: // pre-set volume
    uint32_t volume = LongInt(rx_packet[2], rx_packet[3], rx_packet[4], rx_packet[5]);
    if (volume > gasmeter.volume)
    {
      gasmeter.volume = volume;
    }
    break;
  case 2: // measure request to DSP
    switch (rx_packet[1])
    {
    case 1: // meter reading
      b1 = (uint8_t) (gasmeter.volume >> 24);
      b2 = (uint8_t) (gasmeter.volume >> 16);
      b3 = (uint8_t) (gasmeter.volume >> 8);
      b4 = (uint8_t) (gasmeter.volume & 0xFF);
      break;
    case 2: // temperature
      b3 = (uint8_t) (gasmeter.temperature >> 8);
      b4 = (uint8_t) (gasmeter.temperature & 0xFF);
      break;
    case 3: // humidity
      b3 = (uint8_t) (gasmeter.humidity >> 8);
      b4 = (uint8_t) (gasmeter.humidity & 0xFF);
      break;
    default:
      status = VARIABLE_DOES_NOT_EXIST;
      break;
    }
    break;
  default:
    status = COMMAND_NOT_IMPLEMENTED;
    break;
  }
  SendPacket(status, b1, b2, b3, b4);
  packet_ready = 0;
}
