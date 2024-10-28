#ifndef GasmeterFirmware_h
#define GasmeterFirmware_h

#include "Gasmeter.h"
#include "GasmeterEnums.h"
#include "GasmeterSerial.h"

class GasmeterFirmware {
  static const int SendBufferSize;
  static const int ReceiveBufferSize;

public:
  GasmeterFirmware(void);
  ~GasmeterFirmware(void);
  void SetDebug(const bool &debug);
  bool Setup(const std::string &device, const speed_t baudrate = B19200);
  std::string GetErrorMessage(void) const;
  bool SetMeterVolume(const float &volume);
  bool ClearMeterVolume(void);
  bool SetThresholdLevels(const short int &low_level,
                          const short int &high_level);
  bool ReadDspValue(float &value, const DspValue &type);
  std::string TransmissionState(unsigned char id);

private:
  GasmeterSerial *Serial;
  uint8_t *ReceiveData;
  std::string ErrorMessage;
  bool Send(SendCommand cmd, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4,
            uint8_t b5);
  bool Log;
};

#endif
