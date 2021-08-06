#include <iostream>
#include <iomanip>
#include <cstring>
#include <charconv>
#include <chrono>
#include <thread>
#include <stdexcept>
#include "Gasmeter.h"

const std::set<std::string> Gasmeter::ValidKeys {"mqtt_broker", "mqtt_password", "mqtt_port", "mqtt_topic", "mqtt_user", "mqtt_tls_cafile", "mqtt_tls_capath", "serial_device", "gas_rate", "gas_price", "gas_factor"};

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
  if (!(Cfg->KeyExists("payment_kwh")))
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
  if (!Firmware->ReadDspValue(Datagram.VoltageP1, DspValueEnum::V_IN_1))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Firmware->ReadDspValue(Datagram.CurrentP1, DspValueEnum::I_IN_1))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Firmware->ReadDspValue(Datagram.PowerP1, DspValueEnum::POWER_IN_1))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Firmware->ReadDspValue(Datagram.VoltageP2, DspValueEnum::V_IN_2))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Firmware->ReadDspValue(Datagram.CurrentP2, DspValueEnum::I_IN_2))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Firmware->ReadDspValue(Datagram.PowerP2, DspValueEnum::POWER_IN_2))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Firmware->ReadDspValue(Datagram.GridVoltage, DspValueEnum::GRID_VOLTAGE))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Firmware->ReadDspValue(Datagram.GridCurrent, DspValueEnum::GRID_CURRENT))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Firmware->ReadDspValue(Datagram.GridPower, DspValueEnum::GRID_POWER))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Firmware->ReadDspValue(Datagram.Frequency, DspValueEnum::FREQUENCY))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Firmware->ReadDspValue(Datagram.InverterTemp, DspValueEnum::TEMPERATURE_INVERTER))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Firmware->ReadDspValue(Datagram.BoosterTemp, DspValueEnum::TEMPERATURE_BOOSTER))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  if (!Firmware->ReadDspValue(Datagram.RIso, DspValueEnum::ISOLATION_RESISTANCE))
  {
    ErrorMessage = Firmware->GetErrorMessage();
    return false;
  }
  try
  {
    float denominator = Datagram.PowerP1 + Datagram.PowerP2;
    if (denominator == 0)
    {
      throw std::runtime_error("Math error: Attempted to divide by Zero");
    } 
    Datagram.Efficiency = Datagram.GridPower / denominator * 100.0f;
  }
  catch (std::runtime_error& e)
  {
    ErrorMessage = e.what();
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
    << "\"total_energy\":" << std::setprecision(2) << Datagram.TotalEnergy << "," 
    << "\"voltage_p1\":" << std::setprecision(2) << Datagram.VoltageP1 << ","
    << "\"current_p1\":" << std::setprecision(5) << Datagram.CurrentP1 << ","
    << "\"power_p1\":" << std::setprecision(2) << Datagram.PowerP1 << ","
    << "\"voltage_p2\":" << std::setprecision(2) << Datagram.VoltageP2 << ","
    << "\"current_p2\":" << std::setprecision(5) << Datagram.CurrentP2 << ","
    << "\"power_p2\":" << std::setprecision(2) << Datagram.PowerP2 << ","
    << "\"grid_voltage\":" << std::setprecision(2) << Datagram.GridVoltage << ","
    << "\"grid_current\":" << std::setprecision(5) << Datagram.GridCurrent << ","
    << "\"grid_power\":" << std::setprecision(2) << Datagram.GridPower << ","
    << "\"frequency\":" << std::setprecision(3) << Datagram.Frequency << ","
    << "\"efficiency\":" << std::setprecision(2) << Datagram.Efficiency << ","
    << "\"inverter_temp\":" << std::setprecision(2) << Datagram.InverterTemp << ","
    << "\"booster_temp\":" << std::setprecision(2) << Datagram.BoosterTemp << ","
    << "\"r_iso\":" << std::setprecision(3) << Datagram.RIso << ","
    << "\"payment\":" << Cfg->GetValue("payment_kwh")
    << "},{"
    << "\"serial_num\":\"" << Datagram.SerialNum << "\","
    << "\"part_num\":\"" << Datagram.PartNum << "\","
    << "\"mfg_date\":\"" << Datagram.MfgDate << "\","
    << "\"firmware\":\"" << Datagram.Firmware << "\","
    << "\"inverter_type\":\"" << Datagram.InverterType << "\"," 
    << "\"grid_standard\":\"" << Datagram.GridStandard << "\""
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
