#ifndef Gasmeter_h
#define Gasmeter_h
#include "GasmeterConfig.h"
#include "GasmeterFirmware.h"
#include "GasmeterMqtt.h"

class Gasmeter {
  static const std::set<std::string> ValidKeys;

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

public:
  Gasmeter(void);
  ~Gasmeter(void);
  bool Setup(const std::string &config);
  bool Receive(void);
  bool Publish(void);
  std::string GetErrorMessage(void) const;
  std::string GetPayload(void) const;
  float GetFlowRate(unsigned long long &current_time,
                    float &current_volume) const;
  unsigned char GetLogLevel(void) const;
  bool GetState(float &current_volume) const;

  struct Datagram {
    float Volume; // Gas volume [m³]
    float Energy; // Gas energy [kWh]
    float RawIr;  // Raw value of IR receiver
  } Datagram;
};

#endif
