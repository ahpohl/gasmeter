#ifndef GASMETER_HPP
#define GASMETER_HPP

#include <ctime>
#include <chrono>
#include <mag3110>

class Gasmeter : public MAG3110
{
public:
  Gasmeter(void);
  ~Gasmeter(void);
  void setDebug(void);
  void openI2CDevice(char const* t_device);
  void createFile(char const* t_file, char const* t_socket);
  void runRaw(void) const;

private:
  char const* m_file;           // filename of RRD database
  char const* m_socket;         // socket of rrdcached daemon
  bool m_debug;                 // debug flag
  bool m_raw;                   // flag for raw sensor mode
};

#endif // GASMETER_HPP
