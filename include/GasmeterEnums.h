#ifndef GasmeterEnums_h
#define GasmeterEnums_h

enum class DspValueEnum : unsigned char
{
  GAS_VOLUME = 1,
  DHT22_TEMP = 2,
  DHT22_HUMIDITY = 3
};

/// Send command enum
enum class SendCommandEnum : unsigned char
{
  MEASURE_REQUEST_DSP = 1
};

#endif
