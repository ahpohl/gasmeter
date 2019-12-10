#include <iostream>
#include <thread>
#include <getopt.h>
#include "gasmeter.hpp"

using namespace std;

int main(int argc, char* argv[])
{
  bool help = false;
  bool version = false;
  bool debug = false; 
  char const* i2c_device = nullptr;
  char const* rrd_socket = nullptr;
  char const* rrd_counter = nullptr;
  double gas_counter = 0;
  int trigger_level = 0;
  int trigger_hyst = 0;
  double counter_step = 0;
  char const* rrd_mag = nullptr;

  const struct option longOpts[] = {
    { "help", no_argument, nullptr, 'h' },
    { "version", no_argument, nullptr, 'V' },
    { "debug", no_argument, nullptr, 'D' },
    { "device", required_argument, nullptr, 'd' },
    { "socket", required_argument, nullptr, 's'},
    { "gas", required_argument, nullptr, 'g' },
    { "counter", required_argument, nullptr, 'c'},
    { "level", required_argument, nullptr, 'L' },
    { "hyst", required_argument, nullptr, 'H' },
    { "step", required_argument, nullptr, 'S'},
    { "mag", required_argument, nullptr, 'm' },
    { nullptr, 0, nullptr, 0 }
  };

  const char * optString = "hVDd:s:g:c:L:H:S:m:";

  int opt = 0;
  int longIndex = 0;

  do {
    opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
    switch (opt) {
    case 'h':
      help = true;
      break;
    case 'V':
      version = true;
      break;
    case 'D':
      debug = true;
      break;
    case 'd':
      i2c_device = optarg;
      break;
    case 's':
      rrd_socket = optarg;
      break;
    case 'g':
      rrd_counter = optarg;
      break;
    case 'c':
      gas_counter = atof(optarg);
      break;
    case 'L':
      trigger_level = atoi(optarg);
      break;
    case 'H':
      trigger_hyst = atoi(optarg);
      break;
    case 'S':
      counter_step = atof(optarg);
      break;
    case 'm':
      rrd_mag = optarg;
      break;
    default:
      break;
    }

  } while (opt != -1);

  if (help)
  {
    cout << "Gasmeter " << VERSION_TAG << endl;
    cout << endl << "Usage: " << argv[0] << " [options]" << endl << endl;
    cout << "\
  -h --help              Show help message\n\
  -V --version           Show build info\n\
  -D --debug             Show debug messages\n\
  -d --device [dev]      Set I²C device\n\
  -s --socket [fd]       Set socket of rrdcached daemon\n\
  -L --level [int]       Set trigger level\n\
  -H --hyst [int]        Set trigger hysteresis\n\
\n\
Option 1:\n\
  -g --gas [path]        Save gasmeter counter\n\
  -c --counter [float]   Set intial gas counter [m³]\n\
  -S --step [float]      Set counter step [m³]\n\
\n\
Option 2:\n\
  -m --mag [path]        Save magnetic field data"
    << endl << endl;
    return 0;
  }

  if (version)
  {
      cout << "Version " << VERSION_TAG 
        << " (" << VERSION_BUILD << ") built " 
        << VERSION_BUILD_DATE 
        << " by " << VERSION_BUILD_MACHINE << endl;
      return 0;
  }

  cout << "Gasmeter " << VERSION_TAG
    << " (" << VERSION_BUILD << ")" << endl;

  shared_ptr<Gasmeter> meter(new Gasmeter());

  if (debug) {
    meter->setDebug();
  }

  thread sensor_thread;
  thread mag_thread;
  thread counter_thread;

  meter->openI2CDevice(i2c_device);
  meter->setTriggerParameters(trigger_level, trigger_hyst);
  sensor_thread = thread(&Gasmeter::runSensor, meter);

  if (rrd_mag != nullptr) {
    meter->createFile(rrd_mag, rrd_socket);
    mag_thread = thread(&Gasmeter::runMag, meter);
  }

  if (rrd_counter != nullptr) {
    meter->createFile(rrd_counter, rrd_socket, gas_counter, counter_step);
    counter_thread = thread(&Gasmeter::runCounter, meter);
  }

  if (sensor_thread.joinable()) {
    sensor_thread.join();
  }
  if (mag_thread.joinable()) {
    mag_thread.join();
  }
  if (counter_thread.joinable()) {
    counter_thread.join();
  }

  return 0;
}
