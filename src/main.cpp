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
  char const* ramdisk = nullptr;
  char const* gpio_chip = nullptr;
  double meter_reading = 0;
  double meter_step = 0;
  double gas_factor = 0;
  int trigger_level = 0;
  int trigger_hyst = 0;
  unsigned int gpio_line = 0;

  const struct option longOpts[] = {
    { "help", no_argument, nullptr, 'h' },
    { "version", no_argument, nullptr, 'V' },
    { "debug", no_argument, nullptr, 'D' },
    { "gpiochip", required_argument, nullptr, 'G' },
    { "line", required_argument, nullptr, 'O' },
    { "device", required_argument, nullptr, 'd' },
    { "socket", required_argument, nullptr, 's'},
    { "rrd", required_argument, nullptr, 'r' },
    { "ramdisk", required_argument, nullptr, 'R' },
    { "meter", required_argument, nullptr, 'm'},
    { "step", required_argument, nullptr, 'S'},
    { "factor", required_argument, nullptr, 'f' },
    { "level", required_argument, nullptr, 'L' },
    { "hyst", required_argument, nullptr, 'H' },
    { nullptr, 0, nullptr, 0 }
  };

  const char* const optString = "hVDG:O:d:s:r:R:m:S:f:L:H:";

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
    case 'G':
      gpio_chip = optarg;
      break;
    case 'O':
      gpio_line = atoi(optarg);
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
    case 'R':
      ramdisk = optarg;
      break;
    case 'm':
      meter_reading = atof(optarg);
      break;
    case 'S':
      meter_step = atof(optarg);
      break;
    case 'f':
      gas_factor = atof(optarg);
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
  -G --gpiochip          Set libgpiod gpiochip device\n\
  -O --line              Set libgpiod line offset\n\
  -d --device [dev]      Set MAG3110 I²C device\n\
  -s --socket [fd]       Set socket of rrdcached daemon\n\
  -r --rrd [path]        Set path of gas.rrd database\n\
  -R --ramdisk [dev]     Set shared memory device\n\
  -m --meter [float]     Set intial gas meter [m³]\n\
  -S --step [float]      Set meter step [m³]\n\
  -f --factor [float]    Set gas conversion factor\n\
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
  meter->setupGpioDevice(gpio_chip, gpio_line);
  meter->setTriggerParameters(trigger_level, trigger_hyst);
  meter->createRRD(rrd_path, rrd_socket);
  meter->setMeterReading(meter_reading, meter_step);
  meter->createObisPath(ramdisk, gas_factor);

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
