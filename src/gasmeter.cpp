#include <iostream>
#include <mag3110>
#include <cstring>
#include <chrono>
#include <thread>
#include <wiringPi.h>
#include <unistd.h>
#include <errno.h>
#include "gasmeter.hpp"
#include "rrd.hpp"

using namespace std;

int const Gasmeter::MAG3110_INT_PIN = 7;
bool Gasmeter::isEvent = false;
void Gasmeter::magISR(void)
{
  isEvent = true;
}

Gasmeter::Gasmeter(void)
{
  m_file = nullptr;
  m_socket = nullptr;
	m_debug = false;
  m_raw = false;
}

Gasmeter::~Gasmeter(void)
{
  if (m_debug) {
    cout << "Gasmeter destructor method called" << endl;
  }
}

void Gasmeter::setDebug(void)
{
  m_debug = true;
}

void Gasmeter::openI2CDevice(char const* t_device)
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
  initialize(t_device);
  reset();
  setDR_OS(MAG3110::MAG3110_DR_OS_1_25_128);
  start();
}

void Gasmeter::getMagneticField(void)
{
  int bx, by, bz;
  while (!isEvent) {
    this_thread::sleep_for(chrono::milliseconds(1));
  }
  getMag(&bx, &by, &bz);
  displayMag(bx, by, bz);
  isEvent = false;
}

void Gasmeter::runRaw(void)
{
  while (true) {
    getMagneticField();
  }
}
