#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <ctime>
#include <cmath>
#include <filesystem>

extern "C" {
#include <rrd.h>
#include <rrd_client.h>
}

#include "gasmeter.hpp"
#include "rrd.hpp"

using namespace std;
namespace fs = std::filesystem;

void Gasmeter::createFile(char const* t_file, char const* t_socket,
  double const& t_meter)
{
  if (!t_file) {
    throw runtime_error("RRD file location not set");
  }

  if (!t_socket) {
    throw runtime_error("RRD cached socket not set");
  }

  m_file = t_file;
  m_socket = t_socket;

	time_t timestamp_start = time(nullptr) - 120;
	const int ds_count = 14;
	const int step_size = 60;
	const int no_overwrite = 1;
}
