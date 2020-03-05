#ifndef DallasGPIO_h
#define DallasGPIO_h

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

#include <inttypes.h>
#include <OneWire.h>

// Model IDs
#define DS2413MODEL         0x3A

// Command Codes
#define DS2413_ACCESS_READ  0xF5
#define DS2413_ACCESS_WRITE 0x5A
#define DS2413_ACK_SUCCESS  0xAA

// Error Codes
#define DS2413_ACK_ERROR    0xFF

// For readPowerSupply on oneWire bus
#ifndef nullptr
#define nullptr NULL
#endif

typedef uint8_t DeviceAddress[8];

class DallasGPIO {
public:

	DallasGPIO();
	DallasGPIO(OneWire*);
	DallasGPIO(OneWire*, uint8_t);

	void setOneWire(OneWire*);

    void setPullupPin(uint8_t);

	// initialise bus
	void begin(void);

	// returns the number of devices found on the bus
	uint8_t getDeviceCount(void);

	// returns the number of DS2413 Family devices on bus
	uint8_t getDS2413Count(void);

	// returns true if address is valid
	bool validAddress(const uint8_t*);

	// returns true if address is of the family of sensors the lib supports.
	bool validFamily(const uint8_t* deviceAddress);

	// finds an address at a given index on the bus
	bool getAddress(uint8_t*, uint8_t);

	// set GPIO0 and GPIO1 HIGH or LOW by index
	bool setState(uint8_t deviceIndex, uint8_t gpio0, uint8_t gpio1);

	// set GPIO0 and GPIO1 HIGH or LOW by address
	bool setStateByAddress(const uint8_t* deviceAddress, uint8_t gpio0, uint8_t gpio1, bool polling = false);

	// get GPIO0 and GPIO1 HIGH or LOW by index
    bool getState(uint8_t deviceIndex, uint8_t* gpio0, uint8_t* gpio1);

    // get GPIO0 and GPIO1 HIGH or LOW by address
    bool getStateByAddress(const uint8_t* deviceAddress, uint8_t* gpio0, uint8_t* gpio1, bool polling = false);

	// attempt to determine if the device at the given address is connected to the bus
	bool isConnected(const uint8_t*);

	// attempt to determine if the device at the given address is connected to the bus
	bool isConnected(const uint8_t*, uint8_t*);

	// read device's power requirements
	bool readPowerSupply(const uint8_t* deviceAddress = nullptr);

	// returns true if the bus requires parasite power
	bool isParasitePowerMode(void);

	// Is a conversion complete on the wire? Only applies to the first sensor on the wire.
	bool isConversionComplete(void);

private:
    // error after last polling transaction
	bool lastError = false;
	
	// parasite power on or off
	bool parasite;

	// external pullup
	bool useExternalPullup;
	uint8_t pullupPin;

	// count of devices on the bus
	uint8_t devices;

	// count of DS2413 devices on bus
	uint8_t ds2413Count;

	// Take a pointer to one wire instance
	OneWire* _wire;

    // External pullup control
    void activateExternalPullup(void);
    void deactivateExternalPullup(void);

};
#endif
