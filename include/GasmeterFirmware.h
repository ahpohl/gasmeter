#ifndef GasmeterFirmware_h
#define GasmeterFirmware_h
#include "GasmeterEnums.h"
#include "GasmeterSerial.h"

class GasmeterFirmware
{
  static const int SendBufferSize; ///< Fixed send buffer size (8 bytes)
  static const int ReceiveBufferSize; ///< Fixed receive buffer size (10 bytes)

private:
  GasmeterSerial *Serial; ///< Serial object which handles the communication with the device
  uint8_t *ReceiveData; ///< Array to hold the answer from the device
  std::string ErrorMessage; ///< String which holds the possible error message
  bool Send(SendCommandEnum cmd, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7);

public:
  ~GasmeterFirmware(void);
  bool Setup(const std::string &device, const speed_t baudrate = B19200);
  std::string GetErrorMessage(void) const;
  bool ReadDspValue(float &value, const DspValueEnum &type, const DspGlobalEnum &global = DspGlobalEnum::MODULE_MEASUREMENT);
};

#endif
