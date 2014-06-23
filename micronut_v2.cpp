/*
 *  MicroNut Flight Computer Software
 *
 *  Written by:
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

//#include <Mega_SPI.h>
#//include <spieeprom.h>
#include <TinyGPS.h>
//#include <Flash.h>
//#include <NewSoftSerial.h>
#include <crc16.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>
//#include <Wire.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Pin Mappings
#define PIN_RTTY_ENABLE     4 	// Active High
#define PIN_ONEWIRE         A0	// Handle from OneWire Library
#define PIN_FLASH_CS        10	// Active Low, handled within SPI library
#define PIN_RTTY_SPACE      2
#define PIN_RTTY_MARK       3
#define PIN_PWR_LED         A3

extern void rtty_txstring(const char *str);
extern void txString(const char *string);
extern void configNMEA(void);
extern char *trim(char *str);
extern void feedGPS(void);

// Global Variables

#if 0
SPIEEPROM storage(1); // Object for Flash memory access.
uint8_t FLASH_FLAGS = 0x00;
#define FLASH_ERROR	0
#define FLASH_ERROR_NOTIFIED 1
#define FLASH_CLEARED 2
#endif

// RTTY speed
const int bitRate = 300;

// tinyGPS object
TinyGPS gps;

#if 0
// Temp sensor objects
OneWire oneWire(PIN_ONEWIRE);
DallasTemperature sensors(&oneWire);
#endif

// Internal Temp Sensor Device IDs (MicroNut Boards)
//uint8_t internal[] = {0x28,0xB9,0xF2,0x36,0x03,0x00,0x00,0x0F}; // #1
static const uint8_t internal[] = {0x28,0x19,0x18,0x37,0x03,0x00,0x00,0xBB}; // #2

// External Temp Sensor Device IDs
//uint8_t external[] = {0x28,0xAB,0xDC,0x59,0x02,0x00,0x00,0xC1}; 
//uint8_t external[] = {0x28,0x6A,0x24,0xE3,0x02,0x00,0x00,0x7B}; // Sensor E1
static const uint8_t external[] = {0x28,0xF2,0x9B,0xE3,0x02,0x00,0x00,0x4B};


struct position_record // Total 25 bytes
{
	uint16_t sequence_no;
	uint8_t	hour;
	uint8_t minute;
	uint8_t	second;
	float latitude;
	float longitude;
	uint16_t altitude;
	uint16_t velocity;
	uint8_t sats;
	int8_t temp_internal;
	int8_t temp_external;
	uint8_t custom[5];
};

static struct position_record record1;

/* Output transmit Buffer. */
static char txBuffer[128];

static void generate_string()
{
    char latString[12], longString[12];

    /* max len of 11. */
    sprintf(latString, "%.5f", record1.latitude);
    sprintf(longString, "%.5f", record1.longitude);

    sprintf(txBuffer, "$$HORUS,%u,%02u:%02u:%02d,%s,%s,%u,%u,%u;%d;%d",
                    record1.sequence_no, record1.hour, record1.minute,
                    record1.second, trim(latString), trim(longString),
                    record1.altitude, record1.velocity, record1.sats,
                    record1.temp_internal, record1.temp_external);
}

void setup()
{
    /* Initialize IOs. */
#if 0
    pinMode(PIN_RTTY_ENABLE, OUTPUT);
    pinMode(PIN_RTTY_SPACE, OUTPUT);
    pinMode(PIN_RTTY_MARK, OUTPUT);
    pinMode(PIN_PWR_LED, OUTPUT);
    digitalWrite(PIN_RTTY_ENABLE, HIGH);
    digitalWrite(PIN_RTTY_MARK, HIGH);
    digitalWrite(PIN_RTTY_SPACE, LOW);
    digitalWrite(PIN_PWR_LED, LOW);
    delay(500);
#endif

    rtty_txstring("Booting Up.\n");

#if 0
    // Setup the SPI Flash Memory, if it exists.
    storage.setup();
    storage.clearSR();
    flash_read_pointer();

    if(bitRead(FLASH_FLAGS,FLASH_CLEARED)){
        rtty_txstring("Flash Erased.\n");
        if(flash_test(0x3FFFFF,0xAA)==0){
            rtty_txstring("Flash Test Failed.\n");
            bitSet(FLASH_FLAGS, FLASH_ERROR);
        }else{
            digitalWrite(PIN_PWR_LED, HIGH);
            rtty_txstring("Flash Test Passed.\n");
        }
    }
    print_slots_remaining();
#endif

#if 0
    Serial.begin(9600);
#endif
    configNMEA();
    rtty_txstring("Started GPS.\n");

#if 0
    sensors.begin();
    sensors.requestTemperatures();
#endif
    rtty_txstring("Started Temp Sensors.\n");

    rtty_txstring("Boot Complete.\n");

}

void loop(){
#if 0
    if(bitRead(FLASH_FLAGS, FLASH_ERROR) && (bitRead(FLASH_FLAGS,FLASH_ERROR_NOTIFIED)==0)){
        rtty_txstring("Flash Memory Failure.\n");
        bitSet(FLASH_FLAGS, FLASH_ERROR_NOTIFIED);
    }
#endif

    memset(&record1, 0, sizeof(record1));

    /* Get the GPS data. */
    feedGPS(); /* Up to 3 second delay here. */
    gps.f_get_position(&record1.latitude, &record1.longitude);
    record1.sats = (uint8_t)gps.sats();
    record1.altitude = (unsigned int)gps.f_altitude();
    record1.velocity = (unsigned int)gps.f_speed_kmph();
    gps.crack_datetime(0, 0, 0, &record1.hour, &record1.minute, &record1.second);

    int _intTemp = 0; //sensors.getTempC(internal);
    if (_intTemp!=85 && _intTemp!=127 &&
            _intTemp!=-127 && _intTemp!=999) {
        record1.temp_internal = (int8_t)_intTemp;
    }

    int _extTemp = 0; //sensors.getTempC(external);
    if (_extTemp!=85 && _extTemp!=127 && _extTemp!=-127 && _extTemp!=999) {
        record1.temp_external = (int8_t)_extTemp;
    }

    generate_string();
    txString(txBuffer);

#if 0
    flash_write_record();
    record1.sequence_no++;
    sensors.requestTemperatures();
#endif


#if 0
    delay(1000);
#endif


}




