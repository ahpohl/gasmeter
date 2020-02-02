#include <iostream>
#include <fstream>
#include <filesystem>
#include "gas.hpp"

using namespace std;
namespace fs = std::filesystem;

const char* const Gas::OBIS_GAS_VOLUME = "7-1:1.0.0";
const char* const Gas::OBIS_GAS_ENERGY = "7-1:31.2.0";
const char* const Gas::OBIS_GAS_X_MAG =  "gas_bx";
const char* const Gas::OBIS_GAS_Y_MAG =  "gas_by";
const char* const Gas::OBIS_GAS_Z_MAG =  "gas_bz";

void Gas::createObisPath(const char* const t_ramdisk) const
{
  fs::path dir(t_ramdisk);
  dir /= "obis";
  if (!fs::exists(dir)) {
    fs::create_directories(dir);
    if (m_debug) {
      cout << "Created directory " << dir.string() << endl;
    }
  }
  if (fs::is_directory(dir)) {
    fs::current_path(dir);
  } else {
    throw runtime_error(string("Path '") + dir.string()
      + "' is not a directory");
  }
}
