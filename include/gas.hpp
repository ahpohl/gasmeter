#ifndef GAS_HPP
#define GAS_HPP

#include <mag3110>

class Gas : public MAG3110
{
public:
  static int const MAG3110_INT_PIN;
  static bool isEvent;
  static void magISR(void);
  static int const RUN_METER_INTERVAL;
  static int const RRD_BUFFER_SIZE;
  static int const RRD_DS_LEN;

  Gas(void);
  ~Gas(void);
  void setDebug(void);

  void runMagSensor(void);
  void openI2CDevice(const char* const t_device);
  void setMagneticField(void);
  void getMagneticField(void);
  void setTriggerParameters(int const& t_level, int const& t_hyst);
  void increaseGasCounter(void);
  
  void runGasCounter(void);
  void createRRD(const char* const t_path, const char* const t_socket,
    double const& t_meter, double const& t_step);
  void setGasCounter(void);
  unsigned long getGasCounter(void);

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
  unsigned long m_counter;      // gas counter in [m³ * 1/step]
};

#endif // GAS_HPP
