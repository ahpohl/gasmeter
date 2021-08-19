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

uint32_t LongInt(const uint8_t b3, const uint8_t b2, const uint8_t b1, const uint8_t b0)
{
  return (uint32_t)(b3 & 0xFF) << 24 | (uint32_t)b2 << 16 | (uint32_t)b1 << 8 | (uint32_t)b0;
}

uint16_t Crc16(uint8_t *data, const int offset, const int count)
{
  uint8_t BccLo = 0xFF;
  uint8_t BccHi = 0xFF;

  for (int i = offset; i < count; i++)
  { 
    uint8_t New = data[i] ^ BccLo;
    uint8_t Tmp = New << 4;
    New = Tmp ^ New;
    Tmp = New >> 5;
    BccLo = BccHi;
    BccHi = New ^ Tmp;
    Tmp = New << 3; 
    BccLo = BccLo ^ Tmp;
    Tmp = New >> 4; 
    BccLo = BccLo ^ Tmp;
  }
  
  return Word(~BccHi, ~BccLo);
}
