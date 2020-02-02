#include <iostream>
#include <thread>
#include <getopt.h>
#include "gas.hpp"

using namespace std;

int main(int argc, char* argv[])
{
  bool help = false;
  bool version = false;
  bool debug = false; 
  char const* i2c_device = nullptr;
  char const* rrd_socket = nullptr;
  char const* rrd_path = nullptr;
  double meter_reading = 0;
  double meter_step = 0;
  int trigger_level = 0;
  int trigger_hyst = 0;

  const struct option longOpts[] = {
    { "help", no_argument, nullptr, 'h' },
    { "version", no_argument, nullptr, 'V' },
    { "debug", no_argument, nullptr, 'D' },
    { "device", required_argument, nullptr, 'd' },
    { "socket", required_argument, nullptr, 's'},
    { "rrd", required_argument, nullptr, 'r' },
    { "meter", required_argument, nullptr, 'm'},
    { "step", required_argument, nullptr, 'S'},
    { "level", required_argument, nullptr, 'L' },
    { "hyst", required_argument, nullptr, 'H' },
    { nullptr, 0, nullptr, 0 }
  };

  const char* const optString = "hVDd:s:r:m:S:L:H:";

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
    case 'r':
      rrd_path = optarg;
      break;
    case 'm':
      meter_reading = atof(optarg);
      break;
    case 'S':
      meter_step = atof(optarg);
      break;
    case 'L':
      trigger_level = atoi(optarg);
      break;
    case 'H':
      trigger_hyst = atoi(optarg);
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
  -d --device [dev]      Set MAG3110 I²C device\n\
  -s --socket [fd]       Set socket of rrdcached daemon\n\
  -r --rrd [path]        set path of gas.rrd database\n\
  -m --meter [float]     Set intial gas meter [m³]\n\
  -S --step [float]      Set meter step [m³]\n\
  -L --level [int]       Set trigger level\n\
  -H --hyst [int]        Set trigger hysteresis"
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

  shared_ptr<Gas> meter(new Gas());

  if (debug) {
    meter->setDebug();
  }

  meter->openI2CDevice(i2c_device);
  meter->setTriggerParameters(trigger_level, trigger_hyst);
  meter->createRRD(rrd_path, rrd_socket);
  meter->setMeterReading(meter_reading, meter_step);

  thread sensor_thread;
  sensor_thread = thread(&Gas::runMagSensor, meter);
  thread meter_thread;
  meter_thread = thread(&Gas::runGasCounter, meter);
  if (sensor_thread.joinable()) {
    sensor_thread.join();
  }
  if (meter_thread.joinable()) {
    meter_thread.join();
  }

  return 0;
}
