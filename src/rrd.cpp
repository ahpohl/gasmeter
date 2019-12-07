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

void Gasmeter::createFile(char const* t_file, char const* t_socket)
{
  if (!t_file) {
    throw runtime_error("RRD file location not set");
  }

  if (!t_socket) {
    throw runtime_error("RRD cached socket not set");
  }

  m_file = t_file;
  m_socket = t_socket;

  time_t timestamp_start = time(nullptr) - 10;
	const int ds_count = 4;
	const int step_size = 1;
	const int no_overwrite = 1;

  // store magnetic field strength bx, by, bz:
  // one value per second
  // 86400 rows == 1 day
	
	char const* ds_schema[] = {
	  "DS:bx:GAUGE:2:-30000:30000",
    "DS:by:GAUGE:2:-30000:30000",
    "DS:bz:GAUGE:2:-30000:30000",
    "RRA:AVERAGE:0.5:1:86400",
    nullptr};

  fs::path dir(t_file);
  fs::create_directories(dir.parent_path());

  int ret = rrdc_connect(t_socket);
  if (ret) {
    throw runtime_error(rrd_get_error());
  }

	ret = rrdc_create(t_file, step_size, timestamp_start, no_overwrite, 
		ds_count, ds_schema);
	if (!ret) {
    cout << "Round Robin Database \"" << t_file << "\" created" << endl;
  }

  ret = rrdc_disconnect();
  if (ret) {
    throw runtime_error(rrd_get_error());
  }
}
