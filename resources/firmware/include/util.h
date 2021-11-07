#ifndef UTIL_H
#define UTIL_H

void SendBuffer(const unsigned char *input, size_t len);
void SendValue(uint32_t value);
void SendRaw(int16_t raw, int32_t counts);
uint16_t Crc16(const uint8_t *packet, size_t length);

#endif // UTIL_H
