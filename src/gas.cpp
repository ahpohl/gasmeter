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
}

Gas::~Gas(void)
{
  if (m_debug) {
    cout << "Gasmeter destructor method called" << endl;
  }
  delete[] m_socket;
  delete[] m_rrd;
  delete[] m_ramdisk;
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
