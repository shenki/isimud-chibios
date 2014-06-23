/*
 *  MicroNut Flight Computer Software
 *
 *  GPS Driver
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

#include <stdbool.h>
#include <stdint.h>

#include "stubs.h"

#include <ccan/array_size/array_size.h>

/* GPS PUBX request string. */
static const char *PUBX = "$PUBX,00*33\n";

/* NMEA commands. */
static const char *GLL_OFF = "$PUBX,40,GLL,0,0,0,0*5C\n";
static const char *ZDA_OFF = "$PUBX,40,ZDA,0,0,0,0*44\n";
static const char *GSV_OFF = "$PUBX,40,GSV,0,0,0,0*59\n";
static const char *GSA_OFF = "$PUBX,40,GSA,0,0,0,0*4E\n";
static const char *RMC_OFF = "$PUBX,40,RMC,0,0,0,0*47\n";
static const char *GGA_OFF = "$PUBX,40,GGA,0,0,0,0*5A\n";
static const char *VTG_OFF = "$PUBX,40,VTG,0,0,0,0*5E\n";

/* Navigation mode: Airborne, 1G. */
static const uint8_t set_navmode[] = {
    0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06, 0x03, 0x00, 0x00,
    0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00,
    0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0xDC
};

static uint8_t ackPacket[10] = { 
    0xB5,    // header
    0x62,    // header
    0x05,    // class
    0x01,    // id
    0x02,    // length
    0x00,
    0x00,  // ACK class
    0x00,  // ACK id
    0,       // CK_A
    0       // CK_B
};

/* Send a byte array of UBX protocol to the GPS. */
void sendUBX(const uint8_t *msg, uint8_t len)
{
    const uint8_t *end = msg + len;
    /* Don't use printf as the byte array may contain \0. */
    while (msg < end)
        putc_gps(*msg++);
}

/* Calculate expected UBX ACK packet and parse UBX response from GPS. */
static bool getUBX_ACK(const uint8_t *msg)
{
    uint8_t b;
    uint8_t ackByteID = 0;
    int startTime = millis();

#define ACK_CLASS       6
#define ACK_ID          7
#define ACK_CHKSUM_A    8
#define ACK_CHKSUM_B    9

    /* Construct the expected ACK packet. */
    ackPacket[ACK_CLASS] = msg[2];
    ackPacket[ACK_ID] = msg[3];

    /* Calculate the checksums. */
    for (uint8_t i=2; i<8; i++) {
        ackPacket[ACK_CHKSUM_A] = ackPacket[8] + ackPacket[i];
        ackPacket[ACK_CHKSUM_B] = ackPacket[9] + ackPacket[8];
    }

    while (1) {

        /* Test for success: All packets in order */
        if (ackByteID > 9)
            return true;

        /* Timeout if no valid response in 3 seconds. */
        if (millis() - startTime > 3000)
            return false;

        /* Make sure data is available to read. */
        b = getc_gps();
        if (b != -1) {
            b = getc_gps();

            /* Check that bytes arrive in sequence as per expected
             * ACK packet. */
            if (b == ackPacket[ackByteID])
                ackByteID++;
            else
                ackByteID = 0;  /* Reset and look again, invalid order. */
        }
    }
}


/* Configures the GPS to only use the strings we want. */
void configNMEA(void)
{
    puts_gps(GLL_OFF);
    puts_gps(ZDA_OFF);
    puts_gps(GSV_OFF);
    puts_gps(GSA_OFF);
    puts_gps(RMC_OFF);
    puts_gps(GGA_OFF);
    puts_gps(VTG_OFF);

    /* Set the navigation mode (Airborne, 1G). */
    sendUBX(set_navmode, ARRAY_SIZE(set_navmode));
    getUBX_ACK(set_navmode);
}

// request the PUBX string and feed the GPS
void feedGPS(void)
{
    uint8_t c;
    long startTime;

    puts_gps(PUBX);
    startTime = millis();
    while (millis() < startTime + 3000) {
        c = getc_gps();
        if (c != -1 && gps_encode(c))
                break;
    }
}

