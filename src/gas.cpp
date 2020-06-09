#include <iostream>
#include <fstream>
#include <cstring>
#include <chrono>
#include <thread>
#include <mutex>
#include <ctime>
#include <unistd.h>
#include <errno.h>
#include "gas.hpp"

using namespace std;

Gas::Gas(void)
{
  m_debug = false;
  m_counter = 0;
  m_bx = 0; m_by = 0; m_bz = 0;
  m_level = 0;
  m_hyst = 0;
  m_step = 0;
  m_chip = 0;
  m_line = 0;
}

Gas::~Gas(void)
{
  if (m_debug) {
    cout << "Gasmeter destructor method called" << endl;
  }
  delete[] m_socket;
  delete[] m_rrd;
}

void Gas::setDebug(void)
{
  m_debug = true;
}

void Gas::setTriggerParameters(int const& t_level, int const& t_hyst)
{
  if (!t_level || !t_hyst) {
    throw runtime_error("Trigger parameters not set");
  }
  m_level = t_level;
  m_hyst = t_hyst;
}

void Gas::setGpioDevice(const char* t_chip, unsigned int const& t_line)
{
  if (!t_chip) {
    throw runtime_error("GPIO chip device argument empty");
  }

  if (!t_line) {
    throw runtime_error("GPIO line offset argument empty");
  }

  m_chip = t_chip;
  m_line = t_line;
}

void Gas::runMagSensor(void)
{
  while (true) {
    getMagneticField();
    increaseGasCounter();
    writeObisCodes();
  }
}

void Gas::runGasCounter(void)
{
  while (true) {
    setGasCounter();
    this_thread::sleep_for(chrono::seconds(Gas::RUN_METER_INTERVAL));
  }
}
