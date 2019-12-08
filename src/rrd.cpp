#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <ctime>
#include <cmath>
#include <filesystem>
#include <thread>
#include <mutex>
#include <chrono>

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

  m_rrdmag = t_file;
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

void Gasmeter::createFile(char const* t_file, char const* t_socket, double t_counter, double t_step)
{
  if (!t_file) {
    throw runtime_error("RRD file location not set");
  }

  if (!t_socket) {
    throw runtime_error("RRD cached socket not set");
  }

  m_rrdcounter = t_file;
  m_socket = t_socket;

  time_t timestamp_start = time(nullptr) - 120;
	const int ds_count = 7;
	const int step_size = 60;
	const int no_overwrite = 1;

	// gas is stored in counts (GAUGE)
	// gas [m³] = counts / counter step size
		
	char const* ds_schema[] = {
		"DS:gas:GAUGE:3600:0:U",
		"RRA:LAST:0.5:1:1500",
		"RRA:LAST:0.5:5:900",
    "RRA:LAST:0.5:10:450",
    "RRA:LAST:0.5:15:300",
		"RRA:LAST:0.5:60:750",
		"RRA:LAST:0.5:1440:375",
		nullptr};

	// RRAs
	// keep 1 day in 1 min resolution
	// keep 1 month in 1 hour resolution
	// keep 1 year in 1 day resolution
	// consolidate LAST (gas)

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

void Gasmeter::setMagneticField(void)
{
	time_t timestamp = time(nullptr);
  char* argv[Con::RRD_BUF_SIZE];
	*argv = (char*) malloc(Con::RRD_BUF_SIZE * sizeof(char));

  // rrd format: "timestamp : bx : by : bz"
  memset(*argv, '\0', Con::RRD_BUF_SIZE);
  std::mutex mutex;
  std::lock_guard<std::mutex> guard(mutex);
  snprintf(*argv, Con::RRD_BUF_SIZE, "%ld:%d:%d:%d", timestamp, 
    m_bx, m_by, m_bz);
	
  if (m_debug) {
	  cout << argv[0] << endl;
  }

  int ret = rrdc_connect(m_socket);
  if (ret) {
    throw runtime_error(rrd_get_error());
  }    
    
  ret = rrdc_flush(m_rrdmag);
  if (ret) {
    throw runtime_error(rrd_get_error());
  } 
 
  ret = rrdc_update(m_rrdmag, Con::RRD_DS_LEN, (const char **) argv);
  if (ret) {
    throw runtime_error(rrd_get_error());
  }
  
  ret = rrdc_disconnect();
  if (ret) {
    throw runtime_error(rrd_get_error());
  }

  if (m_debug) {
    ofstream log;
    log.open("mag.log", ios::app);
    struct tm* tm = localtime(&timestamp);
    char time_buffer[32] = {0};
    strftime(time_buffer, 31, "%F %T", tm);

    // Date, Timestamp, bx, by, bz
    log << time_buffer << "," << timestamp << "," << m_bx << ","
      << m_by << "," << m_bz << endl; 
    
    log.close();
  }

	free(*argv);
}

void Gasmeter::setGasCounter(void)
{
  if (m_debug) {
    cout << "set gas counter" << endl;
  }
}

void Gasmeter::runMag(void)
{
  while (true) {
    setMagneticField();
    this_thread::sleep_for(chrono::seconds(1));
  }
}

void Gasmeter::runCounter(void)
{
  while (true) {
    setGasCounter();
    this_thread::sleep_for(chrono::seconds(60));
  }
}