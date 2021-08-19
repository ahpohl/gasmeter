#ifndef UTIL_H
#define UTIL_H

void SendBuffer(const unsigned char *input, size_t len);
void SendValue(uint32_t value);

uint16_t Word(const uint8_t msb, const uint8_t lsb);
uint32_t LongInt(const uint8_t b3, const uint8_t b2, const uint8_t b1, const uint8_t b0);
uint16_t Crc16(uint8_t *data, const int offset, const int count);

#endif // UTIL_H
