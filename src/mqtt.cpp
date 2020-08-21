#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <cmath>
#include "gas.hpp"
#include "mosq.hpp"

void Gas::initMqtt(char const* const t_host, int const& t_port, char const* const t_topic)
{
  if (!t_host) {
    throw std::runtime_error("MQTT broker host argument empty");
  }
  if (!t_port) {
    throw std::runtime_error("MQTT broker port argument empty");
  }
  m_mqtt = new Mosq(t_host, t_port, m_debug ? true : false);
  m_topic = std::string(t_topic);
}

void Gas::publishMqtt(void) const
{
  std::mutex mutex;
  std::lock_guard<std::mutex> guard(mutex);
  std::string topic = m_topic + "/state";
  std::stringstream payload;

  // json string with influxdb fields and tags 
  // magnetization in Gauss, volume in m³, energy in kWh
  payload << "[{"
    << "\"bx\":" << std::fixed << std::setprecision(0) << m_bx << ","
    << "\"by\":" << m_by << ","
    << "\"bz\":" << m_bz << ","
    << "\"mag\":" << std::sqrt(m_bx*m_bx+m_by*m_by+m_bz*m_bz) << ","
    << "\"volume\":" << m_counter*m_step << ","
    << "\"energy\":" << m_counter*m_step*m_factor << ","
    << "},{"
    << "\"factor\":" << m_factor << ","
    << "}]";

  m_mqtt->send_message(topic.c_str(), payload.str().c_str());
  if (m_debug) {
    std::cout << payload.str() << std::endl;
  }
}

void Gas::runMqtt(void) const
{
  while (true) {
    publishMqtt();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}
