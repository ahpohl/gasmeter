#include <stdio.h>
#include <stddef.h>
#include "uart.h"

void send_buffer(unsigned char const* input, size_t len)
{
  char buffer[4];
  for (size_t i = 0; i < len; ++i) {
    snprintf(buffer, 4, "%02x ", input[i]);
    uart_puts(buffer);
  }
  uart_putc('\n');
  uart_putc('\r');
}

void send_value(uint32_t value)
{
  char buffer[9];
  snprintf(buffer, 9, "%08ld", value);
  uart_puts(buffer);
  uart_putc('\n');
  uart_putc('\r');
}
