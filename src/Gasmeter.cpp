#include <iostream>
#include <iomanip>
#include <cstring>
#include <charconv>
#include <chrono>
#include <thread>
#include <stdexcept>
#include "Gasmeter.h"

const std::set<std::string> Gasmeter::ValidKeys {"mqtt_broker", "mqtt_password", "mqtt_port", "mqtt_topic", "mqtt_user", "mqtt_tls_cafile", "mqtt_tls_capath", "serial_device", "gas_rate", "gas_price", "gas_factor", "gas_meter"};

Gasmeter::Gasmeter(const bool &log): Log(log)
{
  Firmware = new GasmeterFirmware();
  Mqtt = new GasmeterMqtt(Log);
  Cfg = new GasmeterConfig();
}

Gasmeter::~Gasmeter(void)
{
  if (Mqtt->GetConnectStatus())
  {
    Mqtt->PublishMessage("offline", Cfg->GetValue("mqtt_topic") + "/status", 1, true);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  if (Mqtt) { delete Mqtt; }
  if (Firmware) { delete Firmware; }
  if (Cfg) { delete Cfg; };
}

bool Gasmeter::Setup(const std::string &config)
{
  if (!Cfg->Begin(config))
  {
    ErrorMessage = Cfg->GetErrorMessage();
    return false;
  }
  if (Log)
  {
    Cfg->ShowConfig();
  }
  if (!Cfg->ValidateKeys(Gasmeter::ValidKeys))
  {
    ErrorMessage = Cfg->GetErrorMessage();
    return false;
  }
  if (!(Cfg->KeyExists("gas_rate")))
  {
    ErrorMessage = Cfg->GetErrorMessage();
    return false;
  }
  if (!(Cfg->KeyExists("gas_price")))
  {
    ErrorMessage = Cfg->GetErrorMessage();
    return false;
  }
  if (!(Cfg->KeyExists("gas_factor")))
  {
    ErrorMessage = Cfg->GetErrorMessage();
    return false;
  }
  if (!(Cfg->KeyExists("gas_meter")))
  {
    ErrorMessage = Cfg->GetErrorMessage();
    return false;
  }
  if (!(Cfg->KeyExists("serial_device")))
  {
    ErrorMessage = Cfg->GetErrorMessage();
    return false;
  }
  if (!Firmware->Setup(Cfg->GetValue("serial_device")))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Firmware->SetMeterVolume(StringTo<float>(Cfg->GetValue("gas_meter"))))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Mqtt->Begin())
  {
    ErrorMessage = Mqtt->GetErrorMessage();
    return false;
  }
  if (!(Cfg->KeyExists("mqtt_topic")))
  {
    ErrorMessage = Cfg->GetErrorMessage();
    return false;
  }
  if (!Mqtt->SetLastWillTestament("offline", Cfg->GetValue("mqtt_topic") + "/status", 1, true))
  {
    ErrorMessage = Mqtt->GetErrorMessage();
    return false;
  }
  if ((Cfg->KeyExists("mqtt_user") && Cfg->KeyExists("mqtt_password")))
  {
    if (!Mqtt->SetUserPassAuth(Cfg->GetValue("mqtt_user"), Cfg->GetValue("mqtt_password")))
    {
      ErrorMessage = Mqtt->GetErrorMessage();
      return false;
    }
  }
  if (Cfg->KeyExists("mqtt_tls_cafile") || Cfg->KeyExists("mqtt_tls_capath"))
  {
    if (!Mqtt->SetTlsConnection(Cfg->GetValue("mqtt_tls_cafile"), Cfg->GetValue("mqtt_tls_capath")))
    {
      ErrorMessage = Mqtt->GetErrorMessage();
      return false;
    }
  }
  if (!(Cfg->KeyExists("mqtt_broker")) || !(Cfg->KeyExists("mqtt_port")) )
  {
    ErrorMessage = Cfg->GetErrorMessage();
    return false;
  }
  if (!Mqtt->Connect(Cfg->GetValue("mqtt_broker"), StringTo<double>(Cfg->GetValue("mqtt_port")), 60))
  {
    ErrorMessage = Mqtt->GetErrorMessage();
    return false;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  if (Mqtt->GetConnectStatus())
  {
    std::cout << "Gasmeter is online." << std::endl;
  }
  if (!Mqtt->PublishMessage("online", Cfg->GetValue("mqtt_topic") + "/status", 1, true))
  {
    ErrorMessage = Mqtt->GetErrorMessage();
    return false;
  }

  return true;
}

bool Gasmeter::Receive(void)
{
  if (!Firmware->ReadDspValue(Datagram.Volume, DspValueEnum::GAS_VOLUME))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Firmware->ReadDspValue(Datagram.Temperature, DspValueEnum::TEMPERATURE))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Firmware->ReadDspValue(Datagram.Humidity, DspValueEnum::HUMIDITY))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  return true;
}

bool Gasmeter::Publish(void)
{
  unsigned long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
  
  std::ios::fmtflags old_settings = Payload.flags();
  Payload.str(std::string());
  Payload.setf(std::ios::fixed, std::ios::floatfield);

  Payload << "[{"
    << "\"time\":" << now << ","
    << "\"volume\":" << std::setprecision(2) << Datagram.Volume << "," 
    << "\"energy\":" << std::setprecision(2) << Datagram.Volume * StringTo<float>(Cfg->GetValue("gas_factor")) << ","
    << "\"temperature\":" << std::setprecision(1) << Datagram.Temperature << ","
    << "\"humidity\":" << std::setprecision(1) << Datagram.Humidity << ","
    << "\"rate\":" << Cfg->GetValue("gas_rate") << ","
    << "\"price\":" << Cfg->GetValue("gas_price") << ","
    << "\"factor\":" << Cfg->GetValue("gas_factor")
    << "}]";

  static bool last_connect_status = true;
  if (Mqtt->GetConnectStatus())
  {
    if (!(Mqtt->PublishMessage(Payload.str(), Cfg->GetValue("mqtt_topic") + "/live", 0, false)))
    {
      ErrorMessage = Mqtt->GetErrorMessage();
      return false;
    }
    if (!last_connect_status)
    {
      if (!(Mqtt->PublishMessage("online", Cfg->GetValue("mqtt_topic") + "/status", 1, true)))
      {
        ErrorMessage = Mqtt->GetErrorMessage();
        return false;
      }
      std::cout << "Gasmeter is online." << std::endl;
    }
  }
  last_connect_status = Mqtt->GetConnectStatus();
 
  Payload.flags(old_settings);
  return true;
}

std::string Gasmeter::GetErrorMessage(void) const
{
  return ErrorMessage;
}

std::string Gasmeter::GetPayload(void) const
{
  return Payload.str();
}

template <typename T>
T Gasmeter::StringTo(const std::string &str) const
{
  T value;
  std::istringstream iss(str);
  iss >> value;
  if (iss.fail())
  {
    return T();
  }
  return value;
}
