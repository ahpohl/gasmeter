#ifndef GASMETER_H
#define GASMETER_H

#define UART_BUFFER_SIZE 8 // serial character buffer
#define TX_SIZE 7          // fixed send message
#define RX_SIZE 8          // fixed receive message

// gasmeter error codes (continue uart.h error codes)
#define UART_CRC_ERROR 0x2000
#define COMMAND_NOT_IMPLEMENTED 0x4000
#define VARIABLE_DOES_NOT_EXIST 0x8000

// packet definition
typedef struct gasmeter {
  int32_t volume;
  int16_t level_low;
  int16_t level_high;
  int16_t adc_value;
} gasmeter_t;

enum dsp { GAS_VOLUME = 1, RAW_IR = 2 };

enum command {
  CLEAR_METER_VOLUME = 1,
  SET_METER_VOLUME = 2,
  SET_THRESHOLDS = 3,
  MEASURE_REQUEST_DSP = 4
};

// global variables
extern gasmeter_t gasmeter;
extern volatile uint8_t packet_ready;
extern uint8_t receive_buffer[RX_SIZE];
extern uint8_t error_code;

// eeprom addresses
extern uint32_t AddrVolume;
extern uint16_t AddrLevelLow;
extern uint16_t AddrLevelHigh;

// functions
void SendPacket(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);
void ReceivePacket(void);
void ProcessPacket(void);
void ReadGasMeter(void);

#endif
