#ifndef CRC16_H
#define CRC16_H
/*
 * 2014 Joel Stanley <joel@jms.id.au>
 *
 * CRC-16-ANSI (ploy 0x8005)
 */

#include <stdint.h>

extern const uint16_t crc16_table[];

static inline uint16_t crc16(uint16_t crc, uint8_t data)
{
    return (crc >> 8) ^ crc16_table[(crc ^ data) & 0xff];
}

#endif /* CRC16_H */
