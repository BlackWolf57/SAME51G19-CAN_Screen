#pragma once

#include "Arduino.h"
#include <Wire.h>
extern TwoWire internal_I2C_;

#define EEPROM_ADDR		   0x57
#define I2C_LOWSPEEDMODE	  10000
#define I2C_STANDARDMODE	 100000
#define I2C_FASTMODE		 400000
#define I2C_FASTMODEPLUS	1000000
#define I2C_HIGHSPEEDMODE	3400000

class CLASS_I2CEEPROM_ {
private:
	byte ReadByte ( size_t _address_ );
	uint8_t WriteByte ( size_t _address_, uint8_t _data_ );

public:
	byte Read ( size_t _address_ );
	size_t Read ( size_t _address_, size_t _size_ );

	uint8_t Write ( size_t _address_, uint8_t _data_ )					{ return ( this -> WriteByte ( _address_, _data_ ) ); }
	uint8_t Write ( size_t _address_, uint16_t _data_ );
	uint8_t Update ( size_t _address_, uint8_t _data_ );
	uint8_t Update ( size_t _address_, uint16_t _data_ );
};

extern CLASS_I2CEEPROM_ eeprom_;
