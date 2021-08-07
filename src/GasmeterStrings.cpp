#include <string>
#include "GasmeterStrings.h"

std::string GasmeterStrings::TransmissionState(unsigned char id)
{
  switch (id)
  {
  case 0:
    return "Everything is OK.";
  case 1:
    return "Command is not implemented";
  case 2:
    return "Variable does not exist";
  case 3:
    return "Variable value is out of range";
  case 4:
    return "EEprom not accessible";
  case 5:
    return "Command not Executed";
  case 6:
    return "The variable is not available, retry";
  case 7:
    return "Unable to read counter";
  case 8:
    return "Temp/humidity sensor error";
  case 9:
    return "CRC checksum error";
  default:
    return "Unknown";
  }
}
