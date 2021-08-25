#ifndef GasmeterEnums_h
#define GasmeterEnums_h

enum class DspValueEnum : unsigned char
{
  GAS_VOLUME = 1,
  TEMPERATURE = 2,
  HUMIDITY = 3
};

enum class SendCommandEnum : unsigned char
{
  SET_METER_VOLUME = 1,
  MEASURE_REQUEST_DSP = 2
};

enum class LogLevelEnum : unsigned char
{
  CONFIG = 0x01,
  JSON = 0x02,
  MQTT = 0x04,
  SERIAL = 0x08
};

#endif
