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
  MAG3110 mySensor;
  mySensor.initialize("/dev/i2c-1");
  mySensor.reset();

  int bx, by, bz;
  mySensor.triggerMeasurement();
  mySensor.getMag(&bx, &by, &bz);
  mySensor.displayMag(bx, by, bz);

  mySensor.calibrate();
 
  int xoff, yoff, zoff;
  mySensor.getOffset(&xoff, &yoff, &zoff);
  cout << "Get offset: " << xoff << ", " << yoff << ", " << zoff << endl;
 
  mySensor.triggerMeasurement();
  mySensor.getMag(&bx, &by, &bz);
  mySensor.displayMag(bx, by, bz);

  if (mySensor.isCalibrated()) {
    double heading = mySensor.getHeading();
    cout << "Heading: " << fixed << setprecision(1) << heading 
      << "°" << endl;
  }
  int temp = mySensor.getTemperature();
  cout << "Temperature: " << temp << "°C" << endl;

  mySensor.setDR_OS(MAG3110::MAG3110_DR_OS_10_128);
  uint8_t dr_os = mySensor.getDR_OS();
  cout << "DR_OS setting: " << static_cast<int>(dr_os) << endl;

  if (mySensor.isActive()) {        
    cout << "Sensor is active" << endl;
  } else {
    cout << "Sensor is in standby" << endl;
  }
  
  ofstream file;
  file.open("mag.txt", ios::app);
  time_t timestamp;
  double mag;

  while (true) {
    mySensor.triggerMeasurement();
    mySensor.getMag(&bx, &by, &bz);
    mag = mySensor.getMagnitude(bx, by, bz);
    mySensor.displayMag(bx, by, bz, mag);
    timestamp = time(nullptr);
    file << timestamp << "," << bx << "," << by << "," << bz 
      << "," << mag << endl;
    this_thread::sleep_for(chrono::seconds(1));
  }
  file.close();

  return 0;
}
