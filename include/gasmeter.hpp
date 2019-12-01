#ifndef GASMETER_HPP
#define GASMETER_HPP

#include <ctime>
#include <chrono>

class Gasmeter
{
public:
  Gasmeter(void);
  ~Gasmeter(void);
  void setDebug(void);
  void createFile(char const* t_file, char const* t_socket,
    double const& t_meter);

private:
  char const* m_file;           // filename of RRD database
  char const* m_socket;         // socket of rrdcached daemon
  bool m_debug;                 // debug flag
  bool m_raw;                   // flag for raw sensor mode
  std::chrono::high_resolution_clock::time_point m_clock; // high resolution clock
};

#endif // GASMETER_HPP
