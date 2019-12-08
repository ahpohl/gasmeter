#ifndef GASMETER_HPP
#define GASMETER_HPP

#include <mag3110>

class Gasmeter : public MAG3110
{
public:
  static int const MAG3110_INT_PIN;
  static bool isEvent;
  static void magISR(void);

  Gasmeter(void);
  ~Gasmeter(void);
  void setDebug(void);
  void openI2CDevice(char const* t_device);
  void createFile(char const* t_file, char const* t_socket);
  void createFile(char const* t_file, char const* t_socket,
    double t_counter, double t_step);
  void runSensor(void);
  void getMagneticField(void);
  void runMag(void);
  void setMagneticField(void);
  void runCounter(void);
  void setGasCounter(void);
  void getGasCounter(void);

private:
  char const* m_rrdcounter;     // counter: filename of RRD database
  char const* m_rrdmag;         // magnetic field: filename of RRD database
  char const* m_socket;         // socket of rrdcached daemon
  bool m_debug;                 // debug flag
  int m_bx;                     // x-axis magnetic field
  int m_by;                     // y-axis magnetic field
  int m_bz;                     // z-axis magnetic field
  size_t m_counter;             // gas counter in [m³ * 1/step]
};

#endif // GASMETER_HPP
