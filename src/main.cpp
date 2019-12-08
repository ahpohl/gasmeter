#include <iostream>
#include <thread>
#include <getopt.h>
#include "gasmeter.hpp"

using namespace std;

int main(int argc, char* argv[])
{
  bool raw_mode = false;
  bool debug = false;
  bool version = false;
  bool help = false;
  double meter_reading = 0;
  char const* rrd_file = nullptr;
  char const* rrdcached_socket = nullptr;
  char const* i2c_device = nullptr;
  int trigger_level_low = 0;
  int trigger_level_high = 0;

  const struct option longOpts[] = {
    { "help", no_argument, nullptr, 'h' },
    { "version", no_argument, nullptr, 'V' },
    { "debug", no_argument, nullptr, 'D' },
    { "device", required_argument, nullptr, 'd' },
    { "raw", no_argument, nullptr, 'R' },
    { "low", required_argument, nullptr, 'L' },
    { "high", required_argument, nullptr, 'H' },
    { "file", required_argument, nullptr, 'f' },
    { "socket", required_argument, nullptr, 's'},
    { "meter", required_argument, nullptr, 'm'},
    { nullptr, 0, nullptr, 0 }
  };

  const char * optString = "hVDd:RL:H:f:s:m:";

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
    case 'R':
      raw_mode = true;
      break;
    case 'L':
      trigger_level_low = atoi(optarg);
      break;
    case 'H':
      trigger_level_high = atoi(optarg);
      break;
    case 'f':
      rrd_file = optarg;
      break;
    case 's':
      rrdcached_socket = optarg;
      break;
    case 'm':
      meter_reading = atof(optarg);
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
  -d --device [dev]      I2C device\n\
  -R --raw               Select raw mode\n\
  -L --low [int]         Set trigger level low\n\
  -H --high [int]        Set trigger level high\n\
  -f --file [path]       Full path to rrd file\n\
  -s --socket [fd]       Set socket of rrdcached daemon\n\
  -m --meter [float]     Set meter reading [kWh]"
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

  thread mag3110_thread;
  thread rrd_thread;

  if (raw_mode) {
    meter->createFile(rrd_file, rrdcached_socket);
    meter->openI2CDevice(i2c_device);
    mag3110_thread = thread(&Gasmeter::runMag3110, meter);
    rrd_thread = thread(&Gasmeter::runRRD, meter);
  }

  if (mag3110_thread.joinable()) {
    mag3110_thread.join();
  }
  if (rrd_thread.joinable()) {
    rrd_thread.join();
  }

  return 0;
}
