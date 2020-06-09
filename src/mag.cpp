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
  struct gpiod_chip* chip;
  struct gpiod_line* line;
  struct gpiod_line_event event;
  int ret = 0;

  chip = gpiod_chip_open(m_chip);
  if (!chip) {
    throw runtime_error(string("GPIO chip") + m_chip + " not found.");
  }
  line = gpiod_chip_get_line(chip, m_line);
  if (!line) {
    gpiod_chip_close(chip);
    throw runtime_error(string("GPIO line") + to_string(m_line) + " could not be opened.");
  }
  ret = gpiod_line_request_rising_edge_events(line, "mag3110");
  if (ret < 0) {
    gpiod_chip_close(chip);
    throw runtime_error("Request events failed");
  }
  do {
    ret = gpiod_line_event_wait(line, NULL);
  } while (ret <= 0);
  ret = gpiod_line_event_read(line, &event);
  if (!ret && (event.event_type == GPIOD_LINE_EVENT_RISING_EDGE)) {
    MAG3110::getMag(&m_bx, &m_by, &m_bz);
    if (m_debug) {
      printf("event: %d timestamp: [%8ld.%09ld]\n", event.event_type, event.ts.tv_sec, event.ts.tv_nsec);
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
  }
  gpiod_chip_close(chip);
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
