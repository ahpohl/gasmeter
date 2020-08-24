#ifndef GAS_HPP
#define GAS_HPP

#include <mag3110>
#include "mosq.hpp"

class Gas : public MAG3110
{
public:
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
  void setupGpioDevice(const char* t_chip, unsigned int const& t_line);
  void getMagneticField(void);
  void setTriggerParameters(int const& t_level, int const& t_hyst);
  void increaseGasCounter(void);
  
  void createRRD(const char* const t_path, const char* const t_socket);
  void setMeterReading(double const& t_meter, double const& t_step);
  double getMeterReading(void) const;
  void setGasCounter(void);
  unsigned long getGasCounter(void) const;
  void runRrdCounter(void);
  
  void createObisPath(const char* const t_ramdisk);
  void writeObisCodes(void) const;
  void runMqtt(void) const;
  void initMqtt(char const* const t_host, int const& t_port, char const* const t_topic);
  void setTariff(double const& t_factor, double const& t_rate, double const& t_price);
  void publishMqtt(void) const;
  

private:
  bool m_debug;                 // debug flag
  char* m_rrd;                  // full path of gas.rrd database
  char* m_socket;               // socket of rrdcached daemon
  int m_bx;                     // x-axis magnetic field
  int m_by;                     // y-axis magnetic field
  int m_bz;                     // z-axis magnetic field
  int m_level;                  // trigger level
  int m_hyst;                   // trigger hysteresis
  double m_step;                // counter step size [m³]
  unsigned long m_counter;      // gas counter in [m³ * 1/step]
  struct gpiod_chip* m_chip;    // libgpiod gpio chip device
  struct gpiod_line* m_line;    // libgpiod gpio line offset
  Mosq* m_mqtt;                 // pointer to mosquitto client object
  std::string m_topic;          // MQTT topic to publish to
  double m_factor;              // gas conversion factor to kWh
  double m_rate;                // gas tariff basic rate per year
  double m_price;               // gas tariff price per kWh
};

#endif // GAS_HPP
