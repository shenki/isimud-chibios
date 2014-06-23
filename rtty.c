/*
 *  MicroNut Flight Computer Software
 *
 *  RTTY Driver
 *
 *  Written by:
 *  Terry Baume <terry@bogaurd.net>
 *  Mark Jessop <lenniethelemming@gmail.com>
 *
 *  This code is based on Robert Harrison's (rharrison@hgf.com) RTTY code as
 *  used in the Icarus project
 *
 *  http://pegasushabproject.org.uk/wiki/doku.php/ideas:notes?s=rtty
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  For a full copy of the GNU General Public License, see
 *  <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stdio.h>

#include <crc16.h>

#include <ch.h>
#include <hal.h>

static const int bitRate = 300;

/* TODO: Assign actual values. */
#define BANK_RTTY_MARK GPIOC
#define BANK_RTTY_SPACE GPIOC
#define PIN_RTTY_MARK 1
#define PIN_RTTY_SPACE 1


/* Transmit a bit as a mark or space. */
static void rtty_txbit(int bit)
{
    systime_t now = chTimeNow();

    if (bit) {
        /* High - mark. */
        palSetPad(BANK_RTTY_SPACE, PIN_RTTY_SPACE);
        palClearPad(BANK_RTTY_MARK, PIN_RTTY_MARK);
    } else {
        /* Low - space. */
        palSetPad(BANK_RTTY_MARK, PIN_RTTY_MARK);
        palClearPad(BANK_RTTY_SPACE, PIN_RTTY_SPACE);
    }

    switch (bitRate) {
    case 200:
        chThdSleepUntil(now + US2ST(5050));
        break;

    case 300:
        chThdSleepUntil(now + US2ST(3400));
        break;

    case 150:
        chThdSleepUntil(now + US2ST(6830));
        break;

    case 100:
        chThdSleepUntil(now + US2ST(10300));
        break;

    default:
        chThdSleepUntil(now + US2ST(10000));
        chThdSleepUntil(now + US2ST(10600));
    }
}

/* Transmit a byte, bit by bit, LSB first
 * ASCII_BIT can be either 7bit or 8bit */
static void rtty_txbyte(char c)
{
    int i;

    /* Start bit. */
    rtty_txbit(0);
    /* Send bits for for char LSB first. */
    for (i = 0; i < 7; i++) {
        if (c & 1)
            rtty_txbit(1);
        else
            rtty_txbit(0);
        c = c >> 1;
    }
    /* Stop bit. */
    rtty_txbit(1);
}

/* Transmit a string, one char at a time. */
void rtty_txstring(const char *string)
{
    //dummySerial.read();
    while (*string)
        rtty_txbyte(*string++);
}

static uint16_t CRC16Sum(const char *string)
{
    char c;
    uint16_t crc = 0xFFFF;

    /* Calculate the sum, ignore $ signs. */
    while (c = *string++)
        if (c != '$')
            crc = crc16(crc, c);

    return crc;
}

/* Transmit a string, log it to SD & produce debug output. */
void txString(const char *string)
{
    palClearPad(GPIOC, GPIOC_LED4);
    palSetPad(GPIOC, GPIOC_LED4);

    /* CRC16 checksum. */
    char txSum[6];
    unsigned int checkSum = CRC16Sum(string);
    sprintf(txSum, "%04X", checkSum);

    /* Log the string. */
#if DEBUG > 1
    debug << F("RTTY: ") << string << "*" << txSum << endl;
    debug << HRULE << endl;
#endif

#if LOGGER > 0
    logger << string << F("*") << txSum << endl;
#endif

    /* We need accurate timing, switch off interrupts. */
    /* TODO: is this the correct way to do this? */
    chSysLock();

    /* TX the string. */
    rtty_txstring(string);
    rtty_txstring("*");
    rtty_txstring(txSum);
    rtty_txstring("\r\n");

    /* Interrupts back on. */
    chSysUnlock();
    palClearPad(GPIOC, GPIOC_LED4);
}

void sendArray(uint8_t *arraydata, uint8_t len)
{
    int i;
    char temp[5];
    unsigned int tempbyte = 0;

    rtty_txstring("Array Bytes:");
    for (i = 0; i < len; i++) {
        /* zero pad the value if necessary. */
        tempbyte = arraydata[i];

        rtty_txstring("0x");
        if (arraydata[i] < 16)
            rtty_txstring("0");
        sprintf(temp, "%X", tempbyte);
        rtty_txstring(temp);
        if (i<7)
            rtty_txstring(",");
    }
    rtty_txstring("}\n");
}
