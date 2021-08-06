#include <string>
#include "GasmeterStrings.h"

std::string GasmeterStrings::TransmissionState(unsigned char id)
{
  switch (id)
  {
  case 0:
    return "Everything is OK.";
  case 51:
    return "Command is not implemented";
  case 52:
    return "Variable does not exist";
  case 53:
    return "Variable value is out of range";
  case 54:
    return "EEprom not accessible";
  case 55:
    return "Not Toggled Service Mode";
  case 56:
    return "Can not send the command to internal micro";
  case 57:
    return "Command not Executed";
  case 58:
    return "The variable is not available, retry";
  default:
    return "Unknown";
  }
}
