#ifndef GasmeterEnums_h
#define GasmeterEnums_h

enum class DspValue : unsigned char { GAS_VOLUME = 1, RAW_IR = 2 };

enum class SendCommand : unsigned char {
  CLEAR_METER_VOLUME = 1,
  SET_METER_VOLUME = 2,
  SET_THRESHOLDS = 3,
  MEASURE_REQUEST_DSP = 4
};

#endif
