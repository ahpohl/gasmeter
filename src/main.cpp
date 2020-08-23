#include <iostream>
#include <thread>
#include <getopt.h>
#include "gas.hpp"

int main(int argc, char* argv[])
{
  bool help = false;
  bool version = false;
  bool debug = false; 
  char const* i2c_device = nullptr;
  char const* rrd_socket = nullptr;
  char const* rrd_path = nullptr;
  char const* ramdisk = nullptr;
  double meter_reading = 0;
  double meter_step = 0;
  double gas_factor = 0;
  int trigger_level = 0;
  int trigger_hyst = 0;
  char const* gpio_chip = nullptr;
  unsigned int gpio_line = 0;
  char const* mqtt_host = nullptr;
  char const* mqtt_topic = nullptr;
  int mqtt_port = 0;

  const struct option longOpts[] = {
    { "help", no_argument, nullptr, 'h' },
    { "version", no_argument, nullptr, 'V' },
    { "debug", no_argument, nullptr, 'D' },
    { "chip", required_argument, nullptr, 'G' },
    { "line", required_argument, nullptr, 'O' },
    { "device", required_argument, nullptr, 'd' },
    { "socket", required_argument, nullptr, 's'},
    { "rrd", required_argument, nullptr, 'r' },
    { "ramdisk", required_argument, nullptr, 'R' },
    { "meter", required_argument, nullptr, 'm'},
    { "step", required_argument, nullptr, 'S'},
    { "factor", required_argument, nullptr, 'f' },
    { "level", required_argument, nullptr, 'L' },
    { "hyst", required_argument, nullptr, 'y' },
    { "host", required_argument, nullptr, 'H' },
    { "port", required_argument, nullptr, 'p' },
    { "topic", required_argument, nullptr, 't' },
    { nullptr, 0, nullptr, 0 }
  };

  const char* const optString = "hVDG:O:d:s:r:R:m:S:f:L:y:H:p:t:";

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
    case 'y':
      trigger_hyst = atoi(optarg);
      break;
    case 'H':
      mqtt_host = optarg;
      break;
    case 'p':
      mqtt_port = atoi(optarg);
      break;
    case 't':
      mqtt_topic = optarg;
      break;
    default:
      break;
    }

  } while (opt != -1);

  if (help)
  {
    std::cout << "Gasmeter " << VERSION_TAG << std::endl;
    std::cout << std::endl << "Usage: " << argv[0] << " [options]" << std::endl << std::endl;
    std::cout << "\
  -h --help              Show help message\n\
  -V --version           Show build info\n\
  -D --debug             Show debug messages\n\
  -G --chip              Set libgpiod gpio chip device\n\
  -O --line              Set libgpiod line offset\n\
  -d --device [dev]      Set MAG3110 I²C device\n\
  -s --socket [fd]       Set socket of rrdcached daemon\n\
  -r --rrd [path]        Set path of gas.rrd database\n\
  -R --ramdisk [dev]     Set shared memory device\n\
  -m --meter [float]     Set intial gas meter [m³]\n\
  -S --step [float]      Set meter step [m³]\n\
  -f --factor [float]    Set gas conversion factor\n\
  -L --level [int]       Set trigger level\n\
  -y --hyst [int]        Set trigger hysteresis\n\
  -H --host              Set MQTT broker host or ip\n\
  -p --port [int]        Set MQTT broker port\n\
  -t --topic             Set MQTT topic to publish"
    << std::endl << std::endl;
    return 0;
  }

  if (version)
  {
    std::cout << "Version " << VERSION_TAG 
      << " (" << VERSION_BUILD << ") built " 
      << VERSION_BUILD_DATE 
      << " by " << VERSION_BUILD_MACHINE << std::endl;
    return 0;
  }

  std::cout << "Gasmeter " << VERSION_TAG
    << " (" << VERSION_BUILD << ")" << std::endl;

  std::shared_ptr<Gas> meter(new Gas());

  if (debug) {
    meter->setDebug();
  }

  meter->openI2CDevice(i2c_device);
  meter->setupGpioDevice(gpio_chip, gpio_line);
  meter->setTriggerParameters(trigger_level, trigger_hyst);
  
  meter->createRRD(rrd_path, rrd_socket);
  meter->setMeterReading(meter_reading, meter_step);
  meter->createObisPath(ramdisk, gas_factor);
  meter->initMqtt(mqtt_host, mqtt_port, mqtt_topic);

  std::thread mag_thread;
  mag_thread = std::thread(&Gas::runMagSensor, meter);
  std::thread mqtt_thread;
  mqtt_thread = std::thread(&Gas::runMqtt, meter);
  std::thread rrd_thread;
  rrd_thread = std::thread(&Gas::runRrdCounter, meter);

  if (mag_thread.joinable()) {
    mag_thread.join();
  }
  if (mqtt_thread.joinable()) {
    mqtt_thread.join();
  }
  if (rrd_thread.joinable()) {
    rrd_thread.join();
  }

  meter->setGasCounter();
  if (debug) {
    std::cout << "Last meter reading: " << meter->getMeterReading() << " m³" << std::endl;
  }

  return 0;
}
