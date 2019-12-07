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
  void runRaw(void);
  void getMagneticField(void);

private:
  char const* m_file;           // filename of RRD database
  char const* m_socket;         // socket of rrdcached daemon
  bool m_debug;                 // debug flag
  bool m_raw;                   // flag for raw sensor mode
};

#endif // GASMETER_HPP
