#include <string>
#include "GasmeterStrings.h"

std::string GasmeterStrings::TransmissionState(unsigned char id)
{
  switch (id)
  {
  case 0x00:
    return "Everything is OK";
  case 0x01:
    return "UART no data";
  case 0x02:
    return "UART buffer overflow";
  case 0x04:
    return "UART parity error";
  case 0x08:
    return "UART overrun error";
  case 0x10:
    return "UART frame error";
  case 0x20:
    return "UART CRC error";
  case 0x40:
    return "Command is not implemented";
  case 0x80:
    return "Variable does not exist";
  default:
    return "Unknown";
  }
}
