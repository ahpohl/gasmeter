#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <mag3110>

using namespace std;

int main(int argc, char** argv)
{
  MAG3110 mag;
  mag.initialize("/dev/i2c-1");
  mag.reset();
  mag.start();

  ofstream file;
  file.open("mag.txt", ios::app);
  time_t timestamp;
  double scalarMag;
  int bx, by, bz;

  while (true) {
    mag.getMag(&bx, &by, &bz);
    scalarMag = mag.getMagnitude(bx, by, bz);
    mag.displayMag(bx, by, bz, scalarMag);
    timestamp = time(nullptr);
    file << timestamp << "," << bx << "," << by << "," << bz 
      << "," << scalarMag << endl;
    this_thread::sleep_for(chrono::seconds(1));
  }
  file.close();

  return 0;
}
