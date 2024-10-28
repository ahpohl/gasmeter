#ifndef Gasmeter_h
#define Gasmeter_h

#include "GasmeterConfig.h"
#include "GasmeterFirmware.h"
#include "GasmeterMqtt.h"

class Gasmeter {
  static const std::set<std::string> ValidKeys;

public:
  Gasmeter(void);
  ~Gasmeter(void);
  bool Setup(const std::string &config);
  bool Receive(void);
  bool Publish(void);
  std::string GetErrorMessage(void) const;
  bool IsLogRaw(void) const;
  std::string GetPayload(void) const;
  float GetFlowRate(unsigned long long &current_time,
                    float &current_volume) const;
  bool GetState(float &current_volume) const;

private:
  GasmeterFirmware *Firmware;
  GasmeterMqtt *Mqtt;
  GasmeterConfig *Cfg;
  std::stringstream Payload;
  std::string Config;
  std::string ErrorMessage;
  unsigned char Log;

  template <typename T> T StringTo(const std::string &str) const;
  void SetLogLevel(void);

  struct GasData_t {
    float Volume; // Gas volume [m³]
    float Energy; // Gas energy [kWh]
    float RawIr;  // Raw value of IR receiver
  } GasData;

  enum class LogLevel : unsigned char {
    CONFIG = 0x01,
    JSON = 0x02,
    MQTT = 0x04,
    SERIAL = 0x08,
    RAW = 0x10
  };
};

#endif
