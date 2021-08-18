#ifndef UTIL_H
#define UTIL_H

extern void SendBuffer(const unsigned char *input, size_t len);
extern void SendValue(uint32_t value);

extern uint16_t Word(const uint8_t *msb, const uint8_t *lsb);
extern uint16_t Crc16(uint8_t *data, const int *offset, const int *count);

#endif // UTIL_H
