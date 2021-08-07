#ifndef GasmeterFirmware_h
#define GasmeterFirmware_h
#include "GasmeterEnums.h"
#include "GasmeterSerial.h"

class GasmeterFirmware
{
  static const int SendBufferSize;
  static const int ReceiveBufferSize;

private:
  GasmeterSerial *Serial;
  uint8_t *ReceiveData;
  std::string ErrorMessage;
  bool Send(SendCommandEnum cmd, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5);

public:
  ~GasmeterFirmware(void);
  bool Setup(const std::string &device, const speed_t baudrate = B19200);
  std::string GetErrorMessage(void) const;
  bool ReadDspValue(float &value, const DspValueEnum &type);
};

#endif
