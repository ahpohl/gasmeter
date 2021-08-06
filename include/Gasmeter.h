#ifndef Gasmeter_h
#define Gasmeter_h
#include "GasmeterMqtt.h"
#include "GasmeterConfig.h"
#include "GasmeterFirmware.h"

class Gasmeter
{
  static const std::set<std::string> ValidKeys;

private:
  GasmeterFirmware *Firmware;
  GasmeterMqtt *Mqtt;
  GasmeterConfig *Cfg;
  std::stringstream Payload;
  std::string Config;
  std::string ErrorMessage;
  bool Log;

  template <typename T>
  T StringTo(const std::string &str) const;
 
public:
  Gasmeter(const bool &log);
  ~Gasmeter(void);
  bool Setup(const std::string &config);
  bool Receive(void);
  bool Publish(void);
  std::string GetErrorMessage(void) const;
  std::string GetPayload(void) const;
  
  struct Datagram
  {
    float Volume;             // Gas volume [m³]
    float Energy;             // Gas energy [kWh]
    float Temperature;        // DHT22 temperature [°C]
    float Humidity;           // DHT22 humidity [%rH]
    float Rate;               // Fixed rate per year [€]
    float Price;              // Price per kWh gas [€]
    float Factor;             // Gas factor
  } Datagram;
};

#endif
