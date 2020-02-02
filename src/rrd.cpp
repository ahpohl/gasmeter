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

#include "gas.hpp"

using namespace std;
namespace fs = std::filesystem;

int const Gas::RUN_METER_INTERVAL = 300;
int const Gas::RRD_BUFFER_SIZE = 64;
int const Gas::RRD_DS_LEN = 1;

void Gas::createRRD(const char* const t_path, const char* const t_socket)
{
  if (!t_path) {
    throw runtime_error("Path of RRD files not set");
  }
  if (!t_socket) {
    throw runtime_error("Socket of RRD cached daemon not set");
  }
  m_socket = new char[strlen(t_socket)+1];
  strncpy(m_socket, t_socket, strlen(t_socket));
  fs::path dir(t_path);
  if (!fs::exists(dir)) {
    fs::create_directories(dir);
  }
  dir /= "gas.rrd";
  m_rrd = new char [dir.string().length()+1];
  strncpy (m_rrd, dir.c_str(), dir.string().length());
  int ret = access(m_rrd, F_OK);
  if (!ret) {
    if (m_debug) {
      cout << "Round Robin Database exists" << endl;
    }
    return;
  }

  time_t timestamp_start = time(nullptr) - 2 * Gas::RUN_METER_INTERVAL;
	const int ds_count = 5;
	const int no_overwrite = 1;

	// gas is stored in counts (GAUGE)
	// gas [m³] = counts / counter step size
		
	char const* ds_schema[] = {
		"DS:gas:GAUGE:3600:0:U",
    "RRA:LAST:0.5:1:2300",
    "RRA:LAST:0.5:2:2300",
    "RRA:LAST:0.5:12:800",
    "RRA:LAST:0.5:288:400",
		nullptr};

  ret = rrdc_connect(t_socket);
  if (ret) {
    throw runtime_error(rrd_get_error());
  }
	ret = rrdc_create(m_rrd, Gas::RUN_METER_INTERVAL, timestamp_start, 
    no_overwrite, ds_count, ds_schema);
	if (!ret) {
    cout << "Round Robin Database \"" << dir.string() << "\" created" << endl;
  }
  ret = rrdc_disconnect();
  if (ret) {
    throw runtime_error(rrd_get_error());
  }
}

void Gas::setMeterReading(double const& t_meter, double const& t_step)
{
  if (!t_step) {
    throw runtime_error("Gas meter step size not set");
  }
  m_step = t_step;
  char * argv[Gas::RRD_BUFFER_SIZE];
  time_t timestamp = time(nullptr) - Gas::RUN_METER_INTERVAL;
  unsigned long requested_counter = lround(t_meter / t_step);
  m_counter = getGasCounter();

  if (m_counter < requested_counter)
  {
	  *argv = (char *) malloc(Gas::RRD_BUFFER_SIZE * sizeof(char));
    memset(*argv, '\0', Gas::RRD_BUFFER_SIZE);
    snprintf(*argv, Gas::RRD_BUFFER_SIZE, "%ld:%ld", timestamp, 
      requested_counter);

    int ret = rrdc_connect(m_socket);
    if (ret) {
      throw runtime_error(rrd_get_error());
    }
    ret = rrdc_update(m_rrd, Gas::RRD_DS_LEN, (const char **) argv);
    if (ret) {
      throw runtime_error(rrd_get_error());
    }
    ret = rrdc_disconnect();
    if (ret) {
      throw runtime_error(rrd_get_error());
    }

    m_counter = requested_counter;
    free(*argv);
  }
  
  cout << "Meter reading: " << fixed << setprecision(2)
    << static_cast<double>(m_counter) * t_step << " m³" << endl;
}

void Gas::setGasCounter(void)
{
	time_t timestamp = time(nullptr);
  char* argv[Gas::RRD_BUFFER_SIZE];
	*argv = (char*) malloc(Gas::RRD_BUFFER_SIZE * sizeof(char));

  // rrd format: "timestamp : gas counter)"
  memset(*argv, '\0', Gas::RRD_BUFFER_SIZE);
  snprintf(*argv, Gas::RRD_BUFFER_SIZE, "%ld:%ld", timestamp, m_counter);
	
  int ret = rrdc_connect(m_socket);
  if (ret) {
    throw runtime_error(rrd_get_error());
  }      
  ret = rrdc_flush(m_rrd);
  if (ret) {
    throw runtime_error(rrd_get_error());
  } 
  ret = rrdc_update(m_rrd, Gas::RRD_DS_LEN, (const char **) argv);
  if (ret) {
    throw runtime_error(rrd_get_error());
  }
  ret = rrdc_disconnect();
  if (ret) {
    throw runtime_error(rrd_get_error());
  }

  if (m_debug) {
    ofstream log;
    log.open("gas.log", ios::app);
    struct tm* tm = localtime(&timestamp);
    char time_buffer[32] = {0};
    strftime(time_buffer, 31, "%F %T", tm);
    double meter_reading = m_counter * m_step;
    log << time_buffer << "," << timestamp << "," << m_counter
      << fixed << setprecision(2) << "," << meter_reading << endl; 
    log.close();
  }

	free(*argv);
}

unsigned long Gas::getGasCounter(void)
{
  int ret = rrdc_connect(m_socket);
  if (ret) {
    throw runtime_error(rrd_get_error());
  }  

  ret = rrdc_flush(m_rrd);
  if (ret) {
    throw runtime_error(rrd_get_error());
  }
  
  char **ds_names = 0;
  char **ds_data = 0;
  time_t last_update = 0;
  unsigned long ds_count = 0;

  ret = rrd_lastupdate_r(m_rrd, &last_update, &ds_count, &ds_names, &ds_data);
  if (ret) {
    throw runtime_error(rrd_get_error());
  }

  ret = rrdc_disconnect();
  if (ret) {
    throw runtime_error(rrd_get_error());
  }  
  
  // ds_data[0]: gas counts
  unsigned long counter = atol(ds_data[0]);

  rrd_freemem(ds_names);
  rrd_freemem(ds_data);

  return counter;
}
