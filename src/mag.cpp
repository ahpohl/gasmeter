#include <iostream>
#include <fstream>
#include <mag3110>
#include <cstring>
#include <chrono>
#include <thread>
#include <mutex>
#include <ctime>
#include <wiringPi.h>
#include <unistd.h>
#include <errno.h>
#include "gas.hpp"

using namespace std;

int const Gas::MAG3110_INT_PIN = 7;
bool Gas::isEvent = false;
void Gas::magISR(void)
{
  isEvent = true;
}

void Gas::openI2CDevice(const char* const t_device)
{
  if (!t_device) {
    throw runtime_error("I2C device argument empty");
  }
  if (wiringPiSetup() < 0)
  {
    throw runtime_error(string("Unable to setup wiringPi: ") +
      + strerror(errno) + " (" + to_string(errno) + ")");
  }
  if (wiringPiISR(MAG3110_INT_PIN, INT_EDGE_RISING, &magISR) < 0)
  {
    throw runtime_error(string("Unable to setup ISR: ") +
      + strerror(errno) + " (" + to_string(errno) + ")");
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
    std::mutex mutex;
    std::lock_guard<std::mutex> guard(mutex);
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
