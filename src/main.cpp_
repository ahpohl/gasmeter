#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <mag3110>
#include <wiringPi.h>
#include <errno.h>

using namespace std;

int const MAG3110_PIN = 7;
static volatile bool isEvent = false;
void magISR(void)
{
  isEvent = true;
}

int main(int argc, char** argv)
{
  if (wiringPiSetup() < 0)
  {
    throw runtime_error(string("Unable to setup wiringPi: ") + 
      + strerror(errno) + " (" + to_string(errno) + ")");
  }
  if (wiringPiISR(MAG3110_PIN, INT_EDGE_RISING, &magISR) < 0)
  {
    throw runtime_error(string("Unable to setup ISR: ") + 
      + strerror(errno) + " (" + to_string(errno) + ")");
  }

  MAG3110 mag;
  mag.getVersion();
  mag.initialize("/dev/i2c-1");
  mag.reset();
  mag.setDR_OS(MAG3110::MAG3110_DR_OS_0_63_128);
  mag.start();
  
  ofstream file;
  file.open("mag.txt", ios::app);
  time_t rawtime;
  struct tm* timeinfo;
  char buffer[80];
  double scalarMag;
  int bx, by, bz;

  while (true) {
    while (!isEvent) {
      this_thread::sleep_for(chrono::milliseconds(1));
    }
    mag.getMag(&bx, &by, &bz);
    scalarMag = mag.getMagnitude(bx, by, bz);
    mag.displayMag(bx, by, bz, scalarMag);
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    file << string(buffer) << "," 
      << bx << "," << by << "," << bz << "," << scalarMag << endl;
    isEvent = false;
  }
  file.close();

  return 0;
}
