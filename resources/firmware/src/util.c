#include <stdio.h>
#include <stddef.h>
#include "uart.h"

void SendBuffer(const unsigned char *input, size_t len)
{
  char buffer[4];
  for (size_t i = 0; i < len; ++i) {
    snprintf(buffer, 4, "%02x ", input[i]);
    uart_puts(buffer);
  }
  uart_putc('\n');
  uart_putc('\r');
}

void SendValue(uint32_t value)
{
  char buffer[9];
  snprintf(buffer, 9, "%08ld", value);
  uart_puts(buffer);
  uart_putc('\n');
  uart_putc('\r');
}

uint16_t Word(const uint8_t msb, const uint8_t lsb)
{
  return ((msb & 0xFF) << 8) | lsb;
}

//  crc16
//                                       16   12   5
//  this is the CCITT CRC 16 polynomial X  + X  + X  + 1.
//  This is 0x1021 when x is 2, but the way the algorithm works
//  we use 0x8408 (the reverse of the bit pattern).  The high
//  bit is always assumed to be set, thus we only use 16 bits to
//  represent the 17 bit value.

uint16_t Crc16Ccitt(const uint8_t *packet, size_t length)
{
  // crc16 polynomial, 1021H bit reversed
  const uint16_t POLY = 0x8408;
  uint16_t crc = 0xffff;
  if (!length) {
    return (~crc);
  }
  uint16_t data;
  uint8_t i;

  do {
    for (i = 0, data = 0xff & *packet++; i < 8; i++, data >>= 1)
    {
      if ((crc & 0x0001) ^ (data & 0x0001)) {
        crc = (crc >> 1) ^ POLY;
      } else {
        crc >>= 1;
      }
    }
  } while (--length);
  crc = ~crc;

  return crc;
}
