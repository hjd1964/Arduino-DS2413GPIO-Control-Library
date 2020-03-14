// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

#include "DallasGPIO.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
extern "C" {
#include "WConstants.h"
}
#endif

DallasGPIO::DallasGPIO()
{
    useExternalPullup = false;
}
DallasGPIO::DallasGPIO(OneWire* _oneWire)
{
    setOneWire(_oneWire);
    useExternalPullup = false;
}

/*
 * Constructs DallasGPIO with strong pull-up turned on. Strong pull-up is mandated in DS2413 datasheet for parasitic
 * power (2 wires) setup. (https://datasheets.maximintegrated.com/en/ds/DS2413.pdf, p. 2, section 'ELECTRICAL CHARACTERISTICS').
 */
DallasGPIO::DallasGPIO(OneWire* _oneWire, uint8_t _pullupPin) : DallasGPIO(_oneWire){
    setPullupPin(_pullupPin);
}

void DallasGPIO::setPullupPin(uint8_t _pullupPin) {
    useExternalPullup = true;
    pullupPin = _pullupPin;
    pinMode(pullupPin, OUTPUT);
    deactivateExternalPullup();
}

void DallasGPIO::setOneWire(OneWire* _oneWire) {
    _wire = _oneWire;
    devices = 0;
    ds2413Count = 0;
    parasite = false;
}

// initialise the bus
void DallasGPIO::begin(void) {
    DeviceAddress deviceAddress;

    _wire->reset_search();
    devices = 0;     // Reset the number of devices when we enumerate wire devices
    ds2413Count = 0; // Reset number of DS2413 device

    while (_wire->search(deviceAddress)) {

        if (validAddress(deviceAddress)) {

            if (!parasite && readPowerSupply(deviceAddress))
                parasite = true;

            devices++;
            if (validFamily(deviceAddress)) {
                ds2413Count++;
            }
        }
    }
}

// returns the number of devices found on the bus
uint8_t DallasGPIO::getDeviceCount(void) {
    return devices;
}

uint8_t DallasGPIO::getDS2413Count(void) {
    return ds2413Count;
}

// returns true if address is valid
bool DallasGPIO::validAddress(const uint8_t* deviceAddress) {
    return (_wire->crc8(deviceAddress, 7) == deviceAddress[7]);
}

bool DallasGPIO::validFamily(const uint8_t* deviceAddress) {
    switch (deviceAddress[0]) {
    case DS2413MODEL:
        return true;
    default:
        return false;
    }
}

// finds an address at a given index on the bus
// returns true if the device was found
bool DallasGPIO::getAddress(uint8_t* deviceAddress, uint8_t index) {

    uint8_t depth = 0;

    _wire->reset_search();

    while (depth <= index && _wire->search(deviceAddress)) {
        if (depth == index && validAddress(deviceAddress))
            return true;
        depth++;
    }

    return false;
}

// set GPIO0 and GPIO1 HIGH or LOW by index
bool DallasGPIO::setState(uint8_t deviceIndex, uint8_t gpio0, uint8_t gpio1) {

    DeviceAddress deviceAddress;
    getAddress(deviceAddress, deviceIndex);

    return setStateByAddress(deviceAddress, gpio0, gpio1);
}

// set GPIO0 and GPIO1 HIGH or LOW by address
bool DallasGPIO::setStateByAddress(const uint8_t* deviceAddress, uint8_t gpio0, uint8_t gpio1, bool polling) {
    static uint8_t stage = 0;
    static uint8_t result = 0;
	static uint8_t state = 0;

    if (stage == 0) {
		lastError = true;
		int b = _wire->reset();
		if (b == 0) { lastError = true; stage=0; return true; } // fail
		stage++;
//		Serial.print("r");
		if (polling) return false;
	}
    if (stage == 1) {
		if (_wire->select(deviceAddress,polling)) stage++;
		if (stage == 1 && !polling) { stage = 0; return true; } // fail
//		if (stage == 2) Serial.print("s,");
		if (polling) return false;
	}
    if (stage == 2) {
		_wire->write(DS2413_ACCESS_WRITE);
		stage++;
//		Serial.print("write,");
		if (polling) return false;
	}
    if (stage == 3) {
		state = 0xFF; if (gpio0) state &= ~0x01; if (gpio1) state &= ~0x02;
		_wire->write(state);
		stage++;
//		Serial.print("*");
		if (polling) return false;
	}
    if (stage == 4) { // invert data and send again
		_wire->write(~state);
		stage++;
//		Serial.print("!,");
		if (polling) return false;
	}
    if (stage == 5) { // read the status byte 0xAA=success, 0xFF=failure
		result = _wire->read();
		stage++;
//		Serial.print("r1:"); Serial.print(result,HEX); Serial.print(",");
		if (polling) return false;
	}
    if (stage == 6) { // check the status byte
		if (result == DS2413_ACK_SUCCESS) result=_wire->read(); else lastError=true;
		stage++;
//		Serial.print("r2:"); Serial.print(result,HEX); Serial.print(",");
		if (polling) return false;
    }
    if (stage == 7) {
		int b = _wire->reset();
		if (b == 0) { lastError = true; stage=0; return true; } // fail
		stage++;
//		Serial.print("r");
		if (polling) return false;
	}

    if (stage == 8) stage = 0;

    return stage == 0;
}


// get GPIO0 and GPIO1 HIGH or LOW by index
bool DallasGPIO::getState(uint8_t deviceIndex, uint8_t* gpio0, uint8_t* gpio1) {

    DeviceAddress deviceAddress;
    getAddress(deviceAddress, deviceIndex);

    return getStateByAddress(deviceAddress, gpio0, gpio1);
}

// get GPIO0 and GPIO1 HIGH or LOW by address
bool DallasGPIO::getStateByAddress(const uint8_t* deviceAddress, uint8_t* gpio0, uint8_t* gpio1, bool polling) {
    static uint8_t stage = 0;
    static uint8_t result = 0;

	if (stage == 0) {
		lastError = false;
		int b = _wire->reset();
		if (b == 0) { lastError = true; stage=0; return true; } // fail
		stage++;
//		Serial.print("r");
		if (polling) return false;
	}
	if (stage == 1) {
		if (_wire->select(deviceAddress,polling)) stage++;
//		if (stage == 2) Serial.print("s,");
		if (polling) return false;
	}
	if (stage == 2) {
		_wire->write(DS2413_ACCESS_READ);
		stage++;
//		Serial.print("read,");
		if (polling) return false;
	}
	if (stage == 3) {
		result = _wire->read();
		stage++;
//		Serial.print("r1:"); Serial.print(result,HEX); Serial.print(",");
		if (polling) return false;
	}
	if (stage == 4) {
		int b = _wire->reset();
		if (b == 0) { lastError = true; stage=0; return true; } // fail
		stage++;
//		Serial.print("r");
		if (polling) return false;
	}
	if (stage == 5) {
		// check inverted nibble
		if (((!result) & 0x0F) == (result >> 4)) {
		  if (result & 1) *gpio0=LOW; else *gpio0=HIGH;
		  if (result & 2) *gpio1=LOW; else *gpio1=HIGH;
		} else lastError=true;
		stage++;
		if (polling) return false;
	}

    if (stage == 6) stage = 0;

    return stage == 0;
}

// attempt to determine if the device at the given address is connected to the bus
bool DallasGPIO::isConnected(const uint8_t* deviceAddress) {
    uint8_t g0, g1;

    return getStateByAddress(deviceAddress, &g0, &g1);
}

// returns true if parasite mode is used (2 wire)
// always true for DS2413
bool DallasGPIO::readPowerSupply(const uint8_t* deviceAddress)
{
    return true;
}

void DallasGPIO::activateExternalPullup() {
    if(useExternalPullup)
        digitalWrite(pullupPin, LOW);
}

void DallasGPIO::deactivateExternalPullup() {
    if(useExternalPullup)
        digitalWrite(pullupPin, HIGH);
}

// returns true if the bus requires parasite power
bool DallasGPIO::isParasitePowerMode(void) {
    return parasite;
}
