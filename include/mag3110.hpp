#ifndef MAG3110_HPP
#define MAG3110_HPP
#include <cstdint>

class MAG3110
{
public:
  static uint8_t const MAG3110_I2C_ADDRESS;
  static uint8_t const MAG3110_WHO_AM_I_RSP;
  static int const CALIBRATION_TIMEOUT;
  static int const MAG3110_DIE_TEMP_OFFSET;
  
  // register addresses
  static uint8_t const MAG3110_DR_STATUS;
  static uint8_t const MAG3110_OUT_X_MSB;
  static uint8_t const MAG3110_OUT_X_LSB;
  static uint8_t const MAG3110_OUT_Y_MSB;
  static uint8_t const MAG3110_OUT_Y_LSB;
  static uint8_t const MAG3110_OUT_Z_MSB;
  static uint8_t const MAG3110_OUT_Z_LSB;
  static uint8_t const MAG3110_WHO_AM_I;
  static uint8_t const MAG3110_SYSMOD;
  static uint8_t const MAG3110_OFF_X_MSB;
  static uint8_t const MAG3110_OFF_X_LSB;
  static uint8_t const MAG3110_OFF_Y_MSB;
  static uint8_t const MAG3110_OFF_Y_LSB;
  static uint8_t const MAG3110_OFF_Z_MSB;
  static uint8_t const MAG3110_OFF_Z_LSB;
  static uint8_t const MAG3110_DIE_TEMP;
  static uint8_t const MAG3110_CTRL_REG1;
  static uint8_t const MAG3110_CTRL_REG2;
  
  // CTRL_REG1 Settings
  // Output Data Rate/Oversample Settings
  // DR_OS_80_16 -> Output Data Rate = 80Hz, 
  // Oversampling Ratio = 16
  static uint8_t const MAG3110_DR_OS_80_16;
  static uint8_t const MAG3110_DR_OS_40_32;
  static uint8_t const MAG3110_DR_OS_20_64;
  static uint8_t const MAG3110_DR_OS_10_128;
  static uint8_t const MAG3110_DR_OS_40_16;
  static uint8_t const MAG3110_DR_OS_20_32;
  static uint8_t const MAG3110_DR_OS_10_64;
  static uint8_t const MAG3110_DR_OS_5_128;
  static uint8_t const MAG3110_DR_OS_20_16;
  static uint8_t const MAG3110_DR_OS_10_32;
  static uint8_t const MAG3110_DR_OS_5_64;
  static uint8_t const MAG3110_DR_OS_2_5_128;
  static uint8_t const MAG3110_DR_OS_10_16;
  static uint8_t const MAG3110_DR_OS_5_32;
  static uint8_t const MAG3110_DR_OS_2_5_64;
  static uint8_t const MAG3110_DR_OS_1_25_128;
  static uint8_t const MAG3110_DR_OS_5_16;
  static uint8_t const MAG3110_DR_OS_2_5_32;
  static uint8_t const MAG3110_DR_OS_1_25_64;
  static uint8_t const MAG3110_DR_OS_0_63_128;
  static uint8_t const MAG3110_DR_OS_2_5_16;
  static uint8_t const MAG3110_DR_OS_1_25_32;
  static uint8_t const MAG3110_DR_OS_0_63_64;
  static uint8_t const MAG3110_DR_OS_0_31_128;
  static uint8_t const MAG3110_DR_OS_1_25_16;
  static uint8_t const MAG3110_DR_OS_0_63_32;
  static uint8_t const MAG3110_DR_OS_0_31_64;
  static uint8_t const MAG3110_DR_OS_0_16_128;
  static uint8_t const MAG3110_DR_OS_0_63_16;
  static uint8_t const MAG3110_DR_OS_0_31_32;
  static uint8_t const MAG3110_DR_OS_0_16_64;
  static uint8_t const MAG3110_DR_OS_0_08_128;

  // Other CTRL_REG1 Settings
  static uint8_t const MAG3110_FAST_READ;
  static uint8_t const MAG3110_TRIGGER_MEASUREMENT;
  static uint8_t const MAG3110_ACTIVE_MODE;

  // CTRL_REG2 Settings
  static uint8_t const MAG3110_AUTO_MRST_EN;
  static uint8_t const MAG3110_RAW_MODE;
  static uint8_t const MAG3110_MAG_RST;

  // SYSMOD Readings
  static uint8_t const MAG3110_SYSMOD_STANDBY;
  static uint8_t const MAG3110_SYSMOD_ACTIVE_RAW;
  static uint8_t const MAG3110_SYSMOD_ACTIVE;

  // axis base address
  static uint8_t const MAG3110_X_AXIS;
  static uint8_t const MAG3110_Y_AXIS;
  static uint8_t const MAG3110_Z_AXIS;

  // DR_STATUS readings
  static uint8_t const MAG3110_DR_STATUS_XDR;
  static uint8_t const MAG3110_DR_STATUS_YDR;
  static uint8_t const MAG3110_DR_STATUS_ZDR;
  static uint8_t const MAG3110_DR_STATUS_XYZDR;
  static uint8_t const MAG3110_DR_STATUS_XOW;
  static uint8_t const MAG3110_DR_STATUS_YOW;
  static uint8_t const MAG3110_DR_STATUS_ZOW;
  static uint8_t const MAG3110_DR_STATUS_XYZOW;

  MAG3110(void);
  ~MAG3110(void);
  void setDebug(bool t_debug);
  void initialize(const char* t_bus);
  uint8_t readRegister(uint8_t const& t_addr) const;
  void writeRegister(uint8_t const& t_addr, uint8_t const& t_val) const;
  void standby(void) const;
  void start(void) const;
  void reset(void);
  bool isActive(void) const;
  bool isRaw(void) const;
  void setRawMode(bool const t_raw) const;
  void triggerMeasurement(void) const;
  bool dataReady(void) const;  
  void setDR_OS(uint8_t const t_DROS);
  uint8_t getDR_OS(void) const;
  void setDelay(uint8_t t_DROS);
  void setDelay(int t_delay);
  int getDelay(void) const;
  void setOffset(int16_t const& t_bxoff, int16_t const& t_byoff, 
    int16_t const& t_bzoff) const;
  void getOffset(int16_t* t_bxoff, int16_t* t_byoff, int16_t* t_bzoff) const;
  void calibrate(void); 
  void readMag(int* t_bx, int* t_by, int* t_bz) const;
  void getMag(int* t_bx, int* t_by, int* t_bz) const;
  int getMagPoll(int* t_bx, int* t_by, int* t_bz) const;
  double getMagnitude(int const& t_bx, int const& t_by, 
    int const& t_bz) const;
  void displayMag(int const& t_bx, int const& t_by, int const& t_bz) const;
  void displayMag(int const& t_bx, int const& t_by, int const& t_bz, 
    double const& t_mag) const;
  int getTemperature(void) const;
  
private:
  bool m_debug;
  int m_fd;
  int m_delay;
};

#endif // MAG3110_HPP
