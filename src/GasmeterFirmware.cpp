#include "Gasmeter.h"
#include "GasmeterEnums.h"
#include "GasmeterSerial.h"
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>

const int GasmeterFirmware::SendBufferSize = 8;
const int GasmeterFirmware::ReceiveBufferSize = 7;

GasmeterFirmware::GasmeterFirmware(void)
    : Serial(nullptr), ReceiveData(0), Log(false) {}

GasmeterFirmware::~GasmeterFirmware(void) {
  if (ReceiveData) {
    delete[] ReceiveData;
  }
  if (Serial) {
    delete Serial;
  }
}

bool GasmeterFirmware::Setup(const std::string &device,
                             const speed_t baudrate) {
  ReceiveData = new uint8_t[GasmeterFirmware::ReceiveBufferSize]();
  Serial = new GasmeterSerial();
  Serial->SetDebug(Log);
  if (!Serial->Begin(device, baudrate)) {
    ErrorMessage = Serial->GetErrorMessage();
    return false;
  }
  return true;
}

std::string GasmeterFirmware::GetErrorMessage(void) const {
  return ErrorMessage;
}

bool GasmeterFirmware::Send(SendCommand cmd, uint8_t b1, uint8_t b2, uint8_t b3,
                            uint8_t b4, uint8_t b5) {
  uint8_t SendData[GasmeterFirmware::SendBufferSize] = {0};

  SendData[0] = static_cast<uint8_t>(cmd);
  SendData[1] = b1;
  SendData[2] = b2;
  SendData[3] = b3;
  SendData[4] = b4;
  SendData[5] = b5;

  uint16_t crc = Serial->Crc16Ccitt(SendData, 6);
  SendData[6] = Serial->LowByte(crc);
  SendData[7] = Serial->HighByte(crc);

  memset(ReceiveData, '\0', GasmeterFirmware::ReceiveBufferSize);

  if (Serial->WriteBytes(SendData, GasmeterFirmware::SendBufferSize) < 0) {
    ErrorMessage =
        std::string("Write bytes failed: ") + Serial->GetErrorMessage();
    Serial->Flush();
    return false;
  }
  if (Serial->ReadBytes(ReceiveData, GasmeterFirmware::ReceiveBufferSize) < 0) {
    ErrorMessage =
        std::string("Read bytes failed: ") + Serial->GetErrorMessage();
    Serial->Flush();
    return false;
  }
  if (!(Serial->Word(ReceiveData[5], ReceiveData[6]) ==
        Serial->Crc16Ccitt(ReceiveData, 5))) {
    ErrorMessage = "Received serial package with CRC mismatch";
    Serial->Flush();
    return false;
  }
  if (ReceiveData[0]) {
    ErrorMessage = std::string("Transmission error: ") +
                   TransmissionState(ReceiveData[0]) + " (" +
                   std::to_string(ReceiveData[0]) + ")";
    return false;
  }
  return true;
}

bool GasmeterFirmware::SetMeterVolume(const float &volume) {
  uint8_t b[4];
  int32_t counts = static_cast<int32_t>(volume * 100);
  memcpy(&b, &counts, sizeof(b));
  if (!Send(SendCommand::SET_METER_VOLUME, 0, b[0], b[1], b[2], b[3])) {
    return false;
  }
  return true;
}

bool GasmeterFirmware::ClearMeterVolume(void) {
  if (!Send(SendCommand::CLEAR_METER_VOLUME, 0, 0, 0, 0, 0)) {
    return false;
  }
  return true;
}

bool GasmeterFirmware::SetThresholdLevels(const short int &low_level,
                                          const short int &high_level) {
  uint8_t l[2], h[2];
  l[0] = static_cast<uint8_t>(low_level);
  l[1] = static_cast<uint8_t>((low_level >> 8) & 0xFF);
  h[0] = static_cast<uint8_t>(high_level);
  h[1] = static_cast<uint8_t>((high_level >> 8) & 0xFF);
  if (!Send(SendCommand::SET_THRESHOLDS, 0, l[0], l[1], h[0], h[1])) {
    return false;
  }
  return true;
}

bool GasmeterFirmware::ReadDspValue(float &value, const DspValue &type) {
  if (!Send(SendCommand::MEASURE_REQUEST_DSP, static_cast<uint8_t>(type), 0, 0,
            0, 0)) {
    return false;
  }
  uint8_t b[] = {ReceiveData[1], ReceiveData[2], ReceiveData[3],
                 ReceiveData[4]};
  int32_t value_int;
  memcpy(&value_int, &b, sizeof(b));
  value = static_cast<float>(value_int) / 100.0;
  return true;
}

std::string GasmeterFirmware::TransmissionState(unsigned char id) {
  switch (id) {
  case 0x00:
    return "Everything is OK";
  case 0x01:
    return "UART no data";
  case 0x02:
    return "UART buffer overflow";
  case 0x04:
    return "UART parity error";
  case 0x08:
    return "UART overrun error";
  case 0x10:
    return "UART frame error";
  case 0x20:
    return "UART CRC error";
  case 0x40:
    return "Command is not implemented";
  case 0x80:
    return "Variable does not exist";
  default:
    return "Unknown";
  }
}
