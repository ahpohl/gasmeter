#ifndef GasmeterEnums_h
#define GasmeterEnums_h

enum class DspValueEnum : unsigned char
{
  GAS_VOLUME = 1,
  TEMPERATURE = 2,
  HUMIDITY = 3,
  RAW_IR = 4
};

enum class SendCommandEnum : unsigned char
{
  SET_METER_VOLUME = 1,
  CLEAR_METER_VOLUME = 2,
  SET_THRESHOLDS = 3,
  MEASURE_REQUEST_DSP = 4
};

enum class LogLevelEnum : unsigned char
{
  CONFIG = 0x01,
  JSON = 0x02,
  MQTT = 0x04,
  SERIAL = 0x08,
  RAW = 0x10
};

#endif
