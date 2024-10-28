#ifndef GasmeterFirmware_h
#define GasmeterFirmware_h

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

  enum class DspValue : unsigned char { GAS_VOLUME = 1, RAW_IR = 2 };
  bool ReadDspValue(float &value, const DspValue &type);
  std::string TransmissionState(unsigned char id);

private:
  GasmeterSerial *Serial;
  uint8_t *ReceiveData;
  std::string ErrorMessage;
  bool Log;

  enum class SendCommand : unsigned char {
    CLEAR_METER_VOLUME = 1,
    SET_METER_VOLUME = 2,
    SET_THRESHOLDS = 3,
    MEASURE_REQUEST_DSP = 4
  };

  bool Send(SendCommand cmd, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4,
            uint8_t b5);
};

#endif
