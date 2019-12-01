#include <iostream>
#include <unistd.h>
#include "gasmeter.hpp"
#include "rrd.hpp"

using namespace std;

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
