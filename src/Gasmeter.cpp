#include <iostream>
#include <iomanip>
#include <cstring>
#include <charconv>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <vector>
#include <unistd.h>
#include "Gasmeter.h"
#include "GasmeterEnums.h"

const std::set<std::string> Gasmeter::ValidKeys {"mqtt_broker", "mqtt_password", "mqtt_port", "mqtt_topic", "mqtt_user", "mqtt_tls_cafile", "mqtt_tls_capath", "serial_device", "gas_rate", "gas_price", "gas_factor", "gas_meter", "gas_force", "log_level", "level_low", "level_high"};

Gasmeter::Gasmeter(void)
{
  Cfg = new GasmeterConfig();
  Firmware = new GasmeterFirmware();
  Mqtt = new GasmeterMqtt();
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
  if (!Cfg->ValidateKeys(Gasmeter::ValidKeys))
  {
    ErrorMessage = Cfg->GetErrorMessage();
    return false;
  }
  this->SetLogLevel();
  Firmware->SetLogLevel(Log);
  Mqtt->SetLogLevel(Log);
  if (Log & static_cast<unsigned char>(LogLevelEnum::CONFIG))
  {
    Cfg->ShowConfig();
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
  if (StringTo<bool>(Cfg->GetValue("gas_force")))
  {
    if (!Firmware->ClearMeterVolume())
    {
      ErrorMessage = Firmware->GetErrorMessage();
      return false;
    }
  }
  if (!Firmware->SetMeterVolume(StringTo<float>(Cfg->GetValue("gas_meter"))))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Firmware->SetThresholdLevels(StringTo<short int>(Cfg->GetValue("level_low")), StringTo<short int>(Cfg->GetValue("level_high"))))
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

void Gasmeter::SetLogLevel(void)
{
  if (Cfg->KeyExists("log_level"))
  {
    std::string line = Cfg->GetValue("log_level");
    std::istringstream iss(line);
    std::string token;
    std::vector<std::string> log_level;

    while(std::getline(iss, token, ','))
    {
      log_level.push_back(token);
    }
    for (auto it = log_level.cbegin(); it != log_level.cend(); ++it)
    {
      if (!(*it).compare("config"))
      {
        Log |= static_cast<unsigned char>(LogLevelEnum::CONFIG);
      }
      else if (!(*it).compare("json"))
      {
        Log |= static_cast<unsigned char>(LogLevelEnum::JSON);
      }
      else if (!(*it).compare("mosquitto"))
      {
        Log |= static_cast<unsigned char>(LogLevelEnum::MQTT);
      }
      else if (!(*it).compare("serial"))
      {
        Log |= static_cast<unsigned char>(LogLevelEnum::SERIAL);
      }
      else if (!(*it).compare("raw"))
      {
        Log |= static_cast<unsigned char>(LogLevelEnum::RAW);
      }
    }
  }
  else
  {
    Log = 0;
  }
  //std::cout << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << ((int)Log & 0xFF) << std::endl;  
}

unsigned char Gasmeter::GetLogLevel(void) const
{
  return Log;
}

bool Gasmeter::Receive(void)
{
  //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  
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
  if (!Firmware->ReadDspValue(Datagram.RawIr, DspValueEnum::RAW_IR))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (Log & static_cast<unsigned char>(LogLevelEnum::RAW))
  {
    //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    //std::cout << (Datagram.RawIr * 100) << " " << Datagram.Volume << " " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()  << std::endl;
    std::cout << (Datagram.RawIr * 100) << " " << Datagram.Volume << std::endl;
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
    << "\"flow\":" << std::setprecision(2) << GetFlowRate(now, Datagram.Volume) << ","
    << "\"temperature\":" << std::setprecision(1) << Datagram.Temperature << ","
    << "\"humidity\":" << std::setprecision(1) << Datagram.Humidity << ","
    << "\"rate\":" << Cfg->GetValue("gas_rate") << ","
    << "\"price\":" << Cfg->GetValue("gas_price") << ","
    << "\"factor\":" << Cfg->GetValue("gas_factor") << ","
    << "\"flame_on\":" << (GetState(Datagram.Volume) ? "1" : "0")
    << "}]";

  if (Log & static_cast<unsigned char>(LogLevelEnum::JSON))
  {
    std::cout << Payload.str() << std::endl;
  }

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

float Gasmeter::GetFlowRate(unsigned long long &current_time, float &current_volume) const
{
  float flow_rate = 0;
  static unsigned long long previous_time = 0;
  static float previous_volume = 0;
  
  if ((current_time - previous_time) > 0)
  {
    flow_rate = (current_volume - previous_volume) / (current_time - previous_time) * 60 * 1000; // L min-1 
  }
  previous_time = current_time;
  previous_volume = current_volume;

  return flow_rate;
}

bool Gasmeter::GetState(float &current_volume) const
{
  static float previous_volume = current_volume;
  bool flame_on = ((current_volume - previous_volume) > 0) ? true : false;
  previous_volume = current_volume;

  return flame_on;
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
