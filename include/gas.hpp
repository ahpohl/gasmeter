#ifndef GAS_HPP
#define GAS_HPP

#include <mag3110>

class Gas : public MAG3110
{
public:
  static bool isEvent;
  static int const RUN_METER_INTERVAL;
  static int const RRD_BUFFER_SIZE;
  static int const RRD_DS_LEN;
  static const char* const OBIS_GAS_VOLUME;
  static const char* const OBIS_GAS_ENERGY;
  static const char* const OBIS_GAS_X_MAG;
  static const char* const OBIS_GAS_Y_MAG;
  static const char* const OBIS_GAS_Z_MAG;

  Gas(void);
  ~Gas(void);
  void setDebug(void);

  void runMagSensor(void);
  void openI2CDevice(const char* const t_device);
  void setGpioDevice(const char* t_chip, unsigned int const& t_line);
  void getMagneticField(void);
  void setTriggerParameters(int const& t_level, int const& t_hyst);
  void increaseGasCounter(void);
  
  void runGasCounter(void);
  void createRRD(const char* const t_path, const char* const t_socket);
  void setMeterReading(double const& t_meter, double const& t_step);
  void setGasCounter(void);
  unsigned long getGasCounter(void);
  void createObisPath(const char* const t_ramdisk, double const& t_factor);
  void writeObisCodes(void) const;

private:
  bool m_debug;                 // debug flag
  char const* m_rrd;            // full path of gas.rrd database
  char const* m_socket;         // socket of rrdcached daemon
  int m_bx;                     // x-axis magnetic field
  int m_by;                     // y-axis magnetic field
  int m_bz;                     // z-axis magnetic field
  int m_level;                  // trigger level
  int m_hyst;                   // trigger hysteresis
  double m_step;                // counter step size [m³]
  double m_factor;              // gas conversion factor to kWh
  unsigned long m_counter;      // gas counter in [m³ * 1/step]
  char const* m_chip;           // libgpiod gpio chip device
  unsigned int m_line;          // libgpiod gpio line offset
};

#endif // GAS_HPP
