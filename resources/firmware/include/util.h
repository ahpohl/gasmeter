#ifndef UTIL_H
#define UTIL_H

void SendBuffer(const unsigned char *input, size_t len);
void SendValue(uint32_t value);

uint16_t Word(const uint8_t msb, const uint8_t lsb);
uint16_t Crc16Ccitt(const uint8_t *packet, size_t length);

#endif // UTIL_H
