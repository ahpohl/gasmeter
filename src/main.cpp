#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <thread>
#include <unistd.h>
#include "mag3110.hpp"

using namespace std;

int main(int argc, char** argv)
{
  MAG3110 mag;
  mag.setDebug(true);
  cout << "Initialization" << endl;
  mag.initialize("/dev/i2c-1");
  cout << "Reset" << endl;
  mag.reset();
  cout << "Start: ";
  mag.start();
  if (mag.isActive()) {
    cout << "Sensor is active" << endl;
  } else {
    cout << "Sensor is in standby" << endl;
  }
  cout << "Standby: ";
  mag.standby();
  if (mag.isActive()) {
    cout << "Sensor is active" << endl;
  } else {
    cout << "Sensor is in standby" << endl;
  }
  int bx, by, bz;
  cout << "Trigger: ";
  mag.triggerMeasurement();
  cout << "Get magnetic induction" << endl;
  mag.getMag(&bx, &by, &bz);
  mag.displayMag(bx, by, bz);
  int16_t bxoff = 500, byoff = -500, bzoff = 1000;
  cout << "Set offset: " << bxoff << " " << byoff << " " << bzoff << endl;
  mag.setOffset(bxoff, byoff, bzoff);
  mag.getOffset(&bxoff, &byoff, &bzoff);
  cout << "Get offset: " << bxoff << ", " << byoff << ", " << bzoff << endl;
  mag.triggerMeasurement();
  mag.getMag(&bx, &by, &bz);
  mag.displayMag(bx, by, bz);
  cout << "Raw: ";
  mag.setRawMode(false);
  if (mag.isRaw()) {
    cout << "Sensor is in raw mode" << endl;
  } else {
    cout << "Sensor is not in raw mode" << endl;
  }
  cout << "Raw: ";
  mag.setRawMode(true);
    if (mag.isRaw()) {
    cout << "Sensor is in raw mode" << endl;
  } else {
    cout << "Sensor is not in raw mode" << endl;
  }
  mag.triggerMeasurement();
  mag.getMag(&bx, &by, &bz);
  mag.displayMag(bx, by, bz);
  mag.calibrate();
  cout << "Calibrate: ";
  mag.getOffset(&bxoff, &byoff, &bzoff);
  cout << bxoff << ", " << byoff << ", " << bzoff << endl;
  mag.getMag(&bx, &by, &bz);
  mag.triggerMeasurement();
  mag.getMag(&bx, &by, &bz);
  mag.displayMag(bx, by, bz);
  int temp = mag.getTemperature();
  cout << "Temperature: " << temp << "°C" << endl;
  mag.setDR_OS(MAG3110::MAG3110_DR_OS_10_128);
  uint8_t dr_os = mag.getDR_OS();
  cout << "DR_OS setting: " << static_cast<int>(dr_os) << endl;
  cout << "Write magnetic moment to file" << endl; 
  ofstream file;
  file.open("mag.txt", ios::app);
  time_t timestamp;
  double scalarMag;

  int delay = mag.getDelay(); 
  cout << "Delay: " << delay << " ms" << endl;
  mag.start();
  this_thread::sleep_for(chrono::milliseconds(delay));

  while (true) {
    mag.getMag(&bx, &by, &bz);
    scalarMag = mag.getMagnitude(bx, by, bz);
    mag.displayMag(bx, by, bz, scalarMag);
    timestamp = time(nullptr);
    file << timestamp << "," << bx << "," << by << "," << bz 
      << "," << scalarMag << endl;
    this_thread::sleep_for(chrono::milliseconds(1000 - delay));
  }
  file.close();

  return 0;
}
