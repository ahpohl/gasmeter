#include <string.h>
#include "uart.h"
#include "util.h"
#include "gasmeter.h"
#include "millis.h"

volatile uint8_t adc_ready = 0;
volatile uint16_t adc_value = 0;
uint8_t rx_packet[RX_SIZE] = {0};
volatile uint8_t packet_ready = 0;
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
    uint16_t crc_payload = Crc16(rx_buffer, RX_SIZE-2);
    if (crc_packet != crc_payload) {
      error_code = (uint8_t) (UART_CRC_ERROR >> 8);
      return;
    }
    memcpy(rx_packet, rx_buffer, RX_SIZE);
    packet_ready = 1;
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

  uint16_t crc_payload = Crc16(tx_buffer, TX_SIZE-2);
  tx_buffer[5] = crc_payload >> 8;
  tx_buffer[6] = crc_payload & 0xFF;
  
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
  if (!packet_ready)
  {
    return;
  }
  uint8_t b[4] = {0};

  switch (rx_packet[0])
  {
  case 1: // pre-set volume
    int32_t volume;
    memcpy(&volume, rx_packet+2, sizeof(volume));
    if (volume > gasmeter.volume)
    {
      gasmeter.volume = volume;
    }
    memcpy(&b, &gasmeter.volume, sizeof(b));
    break;
  case 2: // set threshold levels
    memcpy(&gasmeter.level_high, rx_packet+2, sizeof(gasmeter.level_low));
    memcpy(&gasmeter.level_low, rx_packet+4, sizeof(gasmeter.level_high));
    break;
  case 3: // measure request to DSP
    switch (rx_packet[1])
    {
    case 1: // meter reading
      memcpy(&b, &gasmeter.volume, sizeof(b));
      break;
    case 2: // temperature
      memcpy(&b, &gasmeter.temperature, sizeof(b));
      break;
    case 3: // humidity
      memcpy(&b, &gasmeter.humidity, sizeof(b));
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
  packet_ready = 0;
}

void SendRawAdc(void)
{
  unsigned long current_millis = millis();
  static unsigned long previous_millis = 0;

  if ((current_millis - previous_millis) > 250)
  {
    SendValue(adc_value);
    previous_millis = current_millis;
  }
}

void ReadGasMeter(void)
{
  // check if new ADC value ready
  if (!adc_ready) {
    return;
  }

  static uint8_t hysteresis = 0;
  if (adc_value > gasmeter.level_high)
  {
    hysteresis = 1;
  }
  if ((adc_value < gasmeter.level_low) && hysteresis)
  {
    gasmeter.volume++;
    hysteresis = 0;
  }

  // reset ADC ready flag
  adc_ready = 0;
}
