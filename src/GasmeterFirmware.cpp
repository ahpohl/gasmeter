#include <iostream>
#include <cstring>
#include <sstream>
#include <ctime>
#include "Gasmeter.h"
#include "GasmeterStrings.h"

const int GasmeterFirmware::SendBufferSize = 10;
const int GasmeterFirmware::ReceiveBufferSize = 8;
const time_t GasmeterFirmware::InverterEpoch = 946684800;

GasmeterFirmware::Gasmeter(void) : Address(2)
{
}

GasmeterFirmware::Gasmeter(const unsigned char &addr) : Address(addr)
{
}

GasmeterFirmware::~Gasmeter(void)
{
  if (ReceiveData) { delete[] ReceiveData; }
  if (Serial) { delete Serial; }
}

bool GasmeterFirmware::Setup(const std::string &device, const speed_t baudrate)
{
  ReceiveData = new uint8_t[GasmeterFirmware::ReceiveBufferSize] ();
  Serial = new GasmeterSerial();
  if (!Serial->Begin(device, baudrate))
  {
    ErrorMessage = Serial->GetErrorMessage();
    return false;
  }
  return true;
}

void GasmeterFirmware::SetAddress(const unsigned char &addr)
{
  Address = addr;
}

unsigned char GasmeterFirmware::GetAddress(void) const
{
  return Address;
}

std::string GasmeterFirmware::GetErrorMessage(void) const
{
  return ErrorMessage;
}

bool GasmeterFirmware::Send(SendCommandEnum cmd, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7)
{
  uint8_t SendData[GasmeterFirmware::SendBufferSize] = {0};

  SendData[0] = Address;
  SendData[1] = static_cast<uint8_t>(cmd);
  SendData[2] = b2;
  SendData[3] = b3;
  SendData[4] = b4;
  SendData[5] = b5;
  SendData[6] = b6;
  SendData[7] = b7;

  uint16_t crc = Serial->Crc16(SendData, 0, 8);
  SendData[8] = Serial->LowByte(crc);
  SendData[9] = Serial->HighByte(crc);

  memset(ReceiveData, '\0', GasmeterFirmware::ReceiveBufferSize);

  if (Serial->WriteBytes(SendData, GasmeterFirmware::SendBufferSize) < 0)
  {
    ErrorMessage = std::string("Write bytes failed: ") + Serial->GetErrorMessage();
    Serial->Flush();
    return false;
  }
  if (Serial->ReadBytes(ReceiveData, GasmeterFirmware::ReceiveBufferSize) < 0) 
  {
    ErrorMessage = std::string("Read bytes failed: ") + Serial->GetErrorMessage();
    Serial->Flush();
    return false;
  }
  if (!(Serial->Word(ReceiveData[7], ReceiveData[6]) == Serial->Crc16(ReceiveData, 0, 6)))
  {
    ErrorMessage = "Received serial package with CRC mismatch";
    Serial->Flush();
    return false;
  }
  if ((cmd != SendCommandEnum::PN_READING) && (cmd != SendCommandEnum::SERIAL_NUMBER_READING) && ReceiveData[0])
  {
    ErrorMessage = std::string("Transmission error: ") + GasmeterStrings::TransmissionState(ReceiveData[0]) + " (" + std::to_string(ReceiveData[0]) + ")";
    return false;
  }
  return true;
}

long int GasmeterFirmware::GetGmtOffset(void)
{
  time_t now = time(NULL);
  struct tm tm;
  gmtime_r(&now, &tm);
  tm.tm_isdst = -1;
  time_t gmt = mktime(&tm);

  return difftime(now, gmt);
}

// Inverter commands

bool GasmeterFirmware::ReadState(GasmeterFirmware::State &state)
{
  if (!Send(SendCommandEnum::STATE_REQUEST, 0, 0, 0, 0, 0, 0))
  {
    return false;
  }
  state.GlobalState = GasmeterStrings::GlobalState(ReceiveData[1]);
  state.InverterState = GasmeterStrings::InverterState(ReceiveData[2]);
  state.Channel1State = GasmeterStrings::DcDcState(ReceiveData[3]);
  state.Channel2State = GasmeterStrings::DcDcState(ReceiveData[4]);
  state.AlarmState = GasmeterStrings::AlarmState(ReceiveData[5]);
  return true;
}

bool GasmeterFirmware::ReadPartNumber(std::string &pn)
{
  if (!Send(SendCommandEnum::PN_READING, 0, 0, 0, 0, 0, 0))
  {
    return false;
  }
  std::ostringstream oss;
  for (int c = 0; c < 6; c++)
  {
    oss << ReceiveData[c];
  }
  pn = oss.str();
  return true;
}

bool GasmeterFirmware::ReadVersion(GasmeterFirmware::Version &version)
{
  if (!Send(SendCommandEnum::VERSION_READING, 0, 0, 0, 0, 0, 0))
  {
    return false;
  }
  version.GlobalState = GasmeterStrings::GlobalState(ReceiveData[1]);
  version.Par1 = GasmeterStrings::VersionPart1(ReceiveData[2]);
  version.Par2 = GasmeterStrings::VersionPart2(ReceiveData[3]);
  version.Par3 = GasmeterStrings::VersionPart3(ReceiveData[4]);
  version.Par4 = GasmeterStrings::VersionPart4(ReceiveData[5]);

  return true;
}

bool GasmeterFirmware::ReadDspValue(float &value, const DspValueEnum &type, const DspGlobalEnum &global)
{
  if (!Send(SendCommandEnum::MEASURE_REQUEST_DSP, static_cast<uint8_t>(type), static_cast<uint8_t>(global), 0, 0, 0, 0))
  {
    return false;
  }
  if (ReceiveData[1] != 6) // global state "Run"
  {
    ErrorMessage = std::string("Warning DSP value not trusted: Inverter state \"") + GasmeterStrings::GlobalState(ReceiveData[1]) + "\" (" + std::to_string(ReceiveData[1]) + ")";
    return false;
  }
  uint8_t b[] = {ReceiveData[5], ReceiveData[4], ReceiveData[3], ReceiveData[2]};
  memcpy(&value, &b, sizeof(b));
  return true;
}

bool GasmeterFirmware::ReadSerialNumber(std::string &sn)
{
  if (!Send(SendCommandEnum::SERIAL_NUMBER_READING, 0, 0, 0, 0, 0, 0))
  {
    return false;
  }
  std::ostringstream oss;
  for (int c = 0; c < 6; c++) {
    oss << ReceiveData[c];
  }
  sn = oss.str();

  return true;
}

bool GasmeterFirmware::ReadManufacturingDate(GasmeterFirmware::ManufacturingDate &date)
{
  if (!Send(SendCommandEnum::MANUFACTURING_DATE, 0, 0, 0, 0, 0, 0))
  {
    return false;
  }
  date.GlobalState = GasmeterStrings::GlobalState(ReceiveData[1]);
  std::ostringstream oss;
  oss << ReceiveData[2] << ReceiveData[3];
  date.Week = oss.str();
  oss.str("");
  oss << ReceiveData[4] << ReceiveData[5];
  date.Year = oss.str();

  return true;
}

bool GasmeterFirmware::ReadTimeDate(GasmeterFirmware::TimeDate &date)
{
  if (!Send(SendCommandEnum::TIME_DATE_READING, 0, 0, 0, 0, 0, 0))
  {
    return false;
  }  
  date.GlobalState = GasmeterStrings::GlobalState(ReceiveData[1]);
  uint8_t b[] = {ReceiveData[5], ReceiveData[4], ReceiveData[3], ReceiveData[2]};
  date.InverterTime = 0;
  memcpy(&date.InverterTime, &b, sizeof(b));
  date.EpochTime = date.InverterTime + InverterEpoch - GetGmtOffset();
  struct tm tm;
  localtime_r(&date.EpochTime, &tm);
  char buffer[80];
  strftime(buffer, 80, "%d-%b-%Y %H:%M:%S", &tm);
  date.TimeDate.assign(buffer);
  return true;
}

bool GasmeterFirmware::ReadFirmwareRelease(GasmeterFirmware::FirmwareRelease &firmware)
{
  if (!Send(SendCommandEnum::FIRMWARE_RELEASE_READING, 0, 0, 0, 0, 0, 0))
  {
    return false;
  }
  firmware.GlobalState = GasmeterStrings::GlobalState(ReceiveData[1]);
  std::ostringstream oss;
  for (int c = 2; c < 6; c++)
  {
    oss << ReceiveData[c];
    if (c < 5)
    {
      oss << ".";
    }
  }
  firmware.Release = oss.str();
  return true;
}

bool GasmeterFirmware::ReadCumulatedEnergy(float &cum_energy, const CumulatedEnergyEnum &period)
{
  if (!Send(SendCommandEnum::CUMULATED_ENERGY_READINGS, static_cast<uint8_t>(period), 0, 0, 0, 0, 0))
  {
    return false;
  }
  if (ReceiveData[1] != 6) // global state "Run"
  {
    ErrorMessage = std::string("Warning cumulated energy not trusted: Inverter state \"") + GasmeterStrings::GlobalState(ReceiveData[1]) + "\" (" + std::to_string(ReceiveData[1]) + ")";
    return false;
  }
  uint8_t b[] = {ReceiveData[5], ReceiveData[4], ReceiveData[3], ReceiveData[2]};
  uint32_t energy_watt_hours = 0;
  memcpy(&energy_watt_hours, &b, sizeof(b));
  cum_energy = static_cast<float>(energy_watt_hours) / 1000.0;
  return true;
}

bool GasmeterFirmware::ReadLastFourAlarms(GasmeterFirmware::LastFourAlarms &alarms)
{
  if (!Send(SendCommandEnum::LAST_FOUR_ALARMS, 0, 0, 0, 0, 0, 0))
  {
    return false;
  } 
  alarms.GlobalState = GasmeterStrings::GlobalState(ReceiveData[1]);
  alarms.Alarm1 = GasmeterStrings::AlarmState(ReceiveData[2]);
  alarms.Alarm2 = GasmeterStrings::AlarmState(ReceiveData[3]);
  alarms.Alarm3 = GasmeterStrings::AlarmState(ReceiveData[4]);
  alarms.Alarm4 = GasmeterStrings::AlarmState(ReceiveData[5]);
  return true;
}
