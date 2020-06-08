#include <iostream>
#include <fstream>
#include <mag3110>
#include <cstring>
#include <chrono>
#include <thread>
#include <mutex>
#include <ctime>
#include <gpiod.h>
#include <unistd.h>
#include <errno.h>
#include "gas.hpp"

using namespace std;

bool Gas::isEvent = false;
int event_cb(int event, unsigned int offset, const struct timespec *timestamp, void *unused)
{
  if (event == GPIOD_CTXLESS_EVENT_CB_RISING_EDGE) {
    Gas::isEvent = true;
  }

  return GPIOD_CTXLESS_EVENT_CB_RET_OK;
}

void Gas::setupGpioDevice(const char* t_gpiochip, unsigned int const& t_offset)
{
   
}

void Gas::openI2CDevice(const char* const t_device)
{
  if (!t_device) {
    throw runtime_error("I2C device argument empty");
  }
  MAG3110::initialize(t_device);
  MAG3110::reset();
  MAG3110::setDR_OS(MAG3110::MAG3110_DR_OS_1_25_128);
  MAG3110::start();
}

void Gas::getMagneticField(void)
{
  while (!isEvent) {
    this_thread::sleep_for(chrono::milliseconds(1));
  }
  MAG3110::getMag(&m_bx, &m_by, &m_bz);
  if (m_debug) {
    ofstream log;
    log.open("mag.log", ios::app);
    time_t timestamp = time(nullptr);
    struct tm* tm = localtime(&timestamp);
    char time_buffer[32] = {0};
    strftime(time_buffer, 31, "%F %T", tm);
    log << time_buffer << "," << timestamp << "," << m_bx << ","
      << m_by << "," << m_bz << endl;
    log.close();
  }
  isEvent = false;
}

void Gas::increaseGasCounter(void)
{
  static bool old_state = false;
  bool trigger_state = false;
  if (m_by > (m_level + m_hyst)) {
    trigger_state = true;
  } else if (m_by < (m_level - m_hyst)) {
    trigger_state = false;
  }
  if (!old_state && trigger_state) {
    ++m_counter;
    if (m_debug) {
      ofstream log;
      log.open("count.log", ios::app);
      time_t timestamp = time(nullptr);
      struct tm* tm = localtime(&timestamp);
      char time_buffer[32] = {0};
      strftime(time_buffer, 31, "%F %T", tm);
      log << time_buffer << "," << timestamp << "," << m_counter << endl;
      log.close();
    }
  }
  old_state = trigger_state;
}
