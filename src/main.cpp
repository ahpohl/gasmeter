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
  mySensor.setDebug();
  mySensor.initialize("/dev/i2c-1");
  
  /*
  mySensor.calibrate();
  
  cout << "Raw mode" << endl;
  mySensor.setRawMode(true);
  
  int x, y, z;
  mySensor.readMag(&x, &y, &z);
  mySensor.displayMag(x, y, z); 
  
  int xoff, yoff, zoff;
  mySensor.getOffset(&xoff, &yoff, &zoff);
  cout << "Get offset: " << xoff << ", " << yoff << ", " << zoff << endl;
  mySensor.setRawMode(false);
  this_thread::sleep_for(chrono::milliseconds(100));
  mySensor.readMag(&x, &y, &z);
  mySensor.displayMag(x, y, z);

  if (mySensor.isCalibrated()) {
    double heading = mySensor.getHeading();
    cout << "Heading: " << fixed << setprecision(1) << heading 
      << "°" << endl;
  }
  
  int temp = mySensor.getTemperature();
  cout << "Temperature: " << temp << "°C" << endl;
  */

  mySensor.reset();
  if (mySensor.isActive()) {        
    cout << "Sensor is active" << endl;
  } else {
    cout << "Sensor is in standby" << endl;
  }

  mySensor.setDR_OS(MAG3110::MAG3110_DR_OS_10_128);
  uint8_t dr_os = mySensor.getDR_OS();
  cout << "DR_OS setting: " << static_cast<int>(dr_os) << endl;

  int x, y, z;
  mySensor.readMag(&x, &y, &z);
  mySensor.displayMag(x, y, z); 

  if (mySensor.dataReady()) {
    cout << "New data available" << endl;
  } else {
    cout << "No data available" << endl;
  }

  mySensor.triggerMeasurement();
  if (mySensor.dataReady()) {
    cout << "New data available" << endl;
  } else {
    cout << "No data available" << endl;
  }
  mySensor.readMag(&x, &y, &z);
  mySensor.displayMag(x, y, z);

  /*
  ofstream file;
  file.open("mag.txt", ios::app);
  time_t timestamp;
  mySensor.standby();

  while (true) {
    mySensor.triggerMeasurement();
    int x, y, z;
    if (mySensor.dataReady()) {
      mySensor.readMag2(&x, &y, &z);
    }
    double mag = mySensor.getMagnitude(x, y, z);
    mySensor.displayMag(x, y, z, mag);
    timestamp = time(nullptr);
    file << timestamp << "," << x << "," << y << "," << z 
      << "," << mag << endl;
    sleep(1);
  }

  file.close();
  */

  return 0;
}
