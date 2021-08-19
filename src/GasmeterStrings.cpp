#include <string>
#include "GasmeterStrings.h"

std::string GasmeterStrings::TransmissionState(unsigned char id)
{
  switch (id)
  {
  case 0x00:
    return "Everything is OK.";
  case 0x01:
    return "Command is not implemented";
  case 0x02:
    return "Variable does not exist";
  default:
    return "Unknown";
  }
}
