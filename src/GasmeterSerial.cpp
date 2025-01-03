#include "GasmeterSerial.h"
#include <cstring>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>

GasmeterSerial::GasmeterSerial(void) : SerialPort(0) {}

GasmeterSerial::~GasmeterSerial(void) {
  if (SerialPort) {
    close(SerialPort);
  }
}

bool GasmeterSerial::Begin(const std::string &device, const speed_t &baudrate) {
  if (device.empty()) {
    ErrorMessage = "Serial device argument empty";
    return false;
  }

  SerialPort = open(device.c_str(), O_RDWR | O_NOCTTY);
  if (SerialPort < 0) {
    ErrorMessage = std::string("Error opening device ") + device + ": " +
                   strerror(errno) + " (" + std::to_string(errno) + ")";
    return false;
  }

  int ret = ioctl(SerialPort, TIOCEXCL);
  if (ret < 0) {
    ErrorMessage = std::string("Error getting device lock on") + device + ": " +
                   strerror(errno) + " (" + std::to_string(errno) + ")";
    return false;
  }

  struct termios serial_port_settings;

  memset(&serial_port_settings, 0, sizeof(serial_port_settings));
  ret = tcgetattr(SerialPort, &serial_port_settings);
  if (ret) {
    ErrorMessage = std::string("Error getting serial port attributes: ") +
                   strerror(errno) + " (" + std::to_string(errno) + ")";
    return false;
  }

  cfmakeraw(&serial_port_settings);

  // configure serial port
  // speed: 9600 baud, data bits: 8, stop bits: 1, parity: no
  cfsetispeed(&serial_port_settings, baudrate);
  cfsetospeed(&serial_port_settings, baudrate);

  // set vmin and vtime for non-blocking read
  // vmin: read() returns when x byte(s) are available
  // vtime: wait for up to x * 0.1 second between characters
  serial_port_settings.c_cc[VMIN] = 0;
  serial_port_settings.c_cc[VTIME] = 0;

  // disable reset after modem hangup
  serial_port_settings.c_cflag &= ~HUPCL;

  ret = tcsetattr(SerialPort, TCSANOW, &serial_port_settings);
  if (ret != 0) {
    ErrorMessage = std::string("Error setting serial port attributes: ") +
                   strerror(errno) + " (" + std::to_string(errno) + ")";
    return false;
  }
  tcflush(SerialPort, TCIOFLUSH);
  std::this_thread::sleep_for(std::chrono::milliseconds(1800));

  return true;
}

int GasmeterSerial::ReadBytes(uint8_t *buffer, const int &length) {
  int bytes_received, retval, iterations = 0;
  const int max_iterations = 500;

  while (iterations < max_iterations) {
    int bytes_available;
    retval = ioctl(SerialPort, FIONREAD, &bytes_available);
    if (retval < 0) {
      ErrorMessage = "Serial FIONREAD ioctl failed";
      return -1;
    }
    // intercharacter delay: 1 / baud rate * 1e6 = 17.4 µs
    std::this_thread::sleep_for(std::chrono::microseconds(18));
    if (bytes_available >= length)
      break;
    iterations++;
  }

  if (iterations == max_iterations) {
    ErrorMessage = "Timeout, gasmeter could not be reached";
    return -1;
  }

  bytes_received = read(SerialPort, buffer, length);
  if (bytes_received < 0) {
    ErrorMessage = "Read on serial device failed";
    return -1;
  }

  return bytes_received;
}

int GasmeterSerial::WriteBytes(uint8_t const *buffer, const int &length) {
  int bytes_sent = 0;

  bytes_sent = write(SerialPort, buffer, length);
  if (bytes_sent < 0) {
    ErrorMessage = "Write on serial device failed";
    return -1;
  }
  tcdrain(SerialPort);

  return bytes_sent;
}

void GasmeterSerial::Flush(void) const { tcflush(SerialPort, TCIOFLUSH); }

//  crc16
//                                       16   12   5
//  this is the CCITT CRC 16 polynomial X  + X  + X  + 1.
//  This is 0x1021 when x is 2, but the way the algorithm works
//  we use 0x8408 (the reverse of the bit pattern).  The high
//  bit is always assumed to be set, thus we only use 16 bits to
//  represent the 17 bit value.

uint16_t GasmeterSerial::Crc16Ccitt(const uint8_t *packet,
                                    size_t length) const {
  // crc16 polynomial, 1021H bit reversed
  const uint16_t POLY = 0x8408;
  uint16_t crc = 0xffff;
  if (!length) {
    return (~crc);
  }
  uint16_t data;
  uint8_t i;

  do {
    for (i = 0, data = 0xff & *packet++; i < 8; i++, data >>= 1) {
      if ((crc & 0x0001) ^ (data & 0x0001)) {
        crc = (crc >> 1) ^ POLY;
      } else {
        crc >>= 1;
      }
    }
  } while (--length);
  crc = ~crc;

  return crc;
}

uint8_t GasmeterSerial::LowByte(const uint16_t &bytes) const {
  return static_cast<uint8_t>(bytes);
}

uint8_t GasmeterSerial::HighByte(const uint16_t &bytes) const {
  return static_cast<uint8_t>((bytes >> 8) & 0xFF);
}

uint16_t GasmeterSerial::Word(const uint8_t &msb, const uint8_t &lsb) const {
  return ((msb & 0xFF) << 8) | lsb;
}

std::string GasmeterSerial::GetErrorMessage(void) { return ErrorMessage; }
