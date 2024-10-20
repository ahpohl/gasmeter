#ifndef GasmeterFirmware_h
#define GasmeterFirmware_h
#include "GasmeterEnums.h"
#include "GasmeterSerial.h"

class GasmeterFirmware {
  static const int SendBufferSize;
  static const int ReceiveBufferSize;

private:
  GasmeterSerial *Serial;
  uint8_t *ReceiveData;
  std::string ErrorMessage;
  unsigned char Log;
  bool Send(SendCommandEnum cmd, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4,
            uint8_t b5);

public:
  GasmeterFirmware(void);
  ~GasmeterFirmware(void);
  void SetLogLevel(const unsigned char &log_level);
  bool Setup(const std::string &device, const speed_t baudrate = B19200);
  std::string GetErrorMessage(void) const;
  bool SetMeterVolume(const float &volume);
  bool ClearMeterVolume(void);
  bool SetThresholdLevels(const short int &low_level,
                          const short int &high_level);
  bool ReadDspValue(float &value, const DspValueEnum &type);
  std::string TransmissionState(unsigned char id);
};

#endif
