#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <mutex>
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
  std::stringstream payload, topic;
  std::mutex mutex;
  std::lock_guard<std::mutex> guard(mutex);

  // send volume in m³ 
  payload << std::fixed << std::setprecision(2) << m_counter / m_step;
  topic << m_topic << "/volume/state";
  m_mqtt->send_message(topic.str().c_str(), payload.str().c_str());
  topic=std::stringstream(); payload=std::stringstream();

  // send energy in kWh
  payload << std::fixed << std::setprecision(2) << m_counter / m_step * m_factor;
  topic << m_topic << "/energy/state";
  m_mqtt->send_message(topic.str().c_str(), payload.str().c_str());
  topic=std::stringstream(); payload=std::stringstream();
}

void Gas::runMqtt(void) const
{
  while (true) {
    publishMqtt();
    std::this_thread::sleep_for(std::chrono::minutes(10));
  }
}
