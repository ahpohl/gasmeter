#ifndef GasmeterEnums_h
#define GasmeterEnums_h

enum class DspValueEnum : unsigned char
{
  GAS_VOLUME = 1,
  TEMPERATURE = 2,
  HUMIDITY = 3
};

/// Send command enum
enum class SendCommandEnum : unsigned char
{
  SET_METER_VOLUME = 1,
  MEASURE_REQUEST_DSP = 2
};

#endif
