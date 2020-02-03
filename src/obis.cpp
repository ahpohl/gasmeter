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

void Gas::createObisPath(const char* const t_ramdisk, double const& t_factor)
{
  if (!t_ramdisk) {
    throw runtime_error("Shared memory device not set");
  }
  if (!t_factor) {
    throw runtime_error("Gas conversion factor not set");
  }
  m_factor = t_factor;
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

void Gas::writeObisCodes(void) const
{
  ofstream ofs;
  ofs.open(Gas::OBIS_GAS_X_MAG, ios::out);
  ofs << Gas::OBIS_GAS_X_MAG << "(" << m_bx / 10.0 << "*µT)";
  ofs.close();
  ofs.open(Gas::OBIS_GAS_Y_MAG, ios::out);
  ofs << Gas::OBIS_GAS_Y_MAG << "(" << m_by / 10.0 << "*µT)";
  ofs.close();
  ofs.open(Gas::OBIS_GAS_Z_MAG, ios::out);
  ofs << Gas::OBIS_GAS_Z_MAG << "(" << m_bz / 10.0 << "*µT)";
  ofs.close();
  ofs.open(Gas::OBIS_GAS_VOLUME, ios::out);
  ofs << Gas::OBIS_GAS_VOLUME << "(" << fixed << setprecision(2) 
    << m_counter / m_step << "*m3)";
  ofs.open(Gas::OBIS_GAS_ENERGY, ios::out);
  ofs << Gas::OBIS_GAS_ENERGY << "(" << fixed << setprecision(2)
    << m_counter / m_step * m_factor * 1.0e3 << "*Wh)";
  ofs.close();
}

