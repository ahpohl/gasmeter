#include <iostream>
#include <cstring>
#include <sstream>
#include <ctime>
#include "Gasmeter.h"
#include "GasmeterStrings.h"

const int GasmeterFirmware::SendBufferSize = 10;
const int GasmeterFirmware::ReceiveBufferSize = 8;

GasmeterFirmware::~GasmeterFirmware(void)
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

std::string GasmeterFirmware::GetErrorMessage(void) const
{
  return ErrorMessage;
}

bool GasmeterFirmware::Send(SendCommandEnum cmd, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7)
{
  uint8_t SendData[GasmeterFirmware::SendBufferSize] = {0};

  SendData[0] = 0;
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
