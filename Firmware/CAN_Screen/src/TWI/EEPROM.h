#pragma once

#include "Arduino.h"
#include <Wire.h>
extern TwoWire internal_I2C_;

#define EEPROM_ADDR		   0x57
#define EEPROM_PAGE_SIZE	     32
#define I2C_LOWSPEEDMODE	  10000
#define I2C_STANDARDMODE	 100000
#define I2C_FASTMODE		 400000
#define I2C_FASTMODEPLUS	1000000
#define I2C_HIGHSPEEDMODE	3400000

class CLASS_I2CEEPROM_ {
private:
	//byte ReadByte ( size_t _address_ );
	//uint8_t WriteByte ( size_t _address_, uint8_t _data_ );
	uint8_t ReadBlock ( size_t _address_, uint8_t* _buffer_, size_t _length_ );
	uint8_t WriteBlock ( size_t _address_, const uint8_t* _buffer_, size_t _length_ );

public:
	//byte Read ( size_t _address_ )								{ return this -> ReadByte ( _address_ ); }
	//size_t Read ( size_t _address_, size_t _size_ );

	//uint8_t Write ( size_t _address_, uint8_t _data_ )					{ return ( this -> WriteByte ( _address_, _data_ ) ); }
	//uint8_t Write ( size_t _address_, uint16_t _data_ );
	//uint8_t Update ( size_t _address_, uint8_t _data_ );
	//uint8_t Update ( size_t _address_, uint16_t _data_ );

	template < typename T >
	uint8_t Write ( size_t _address_, const T& _data_ )					{ return this -> WriteBlock ( _address_, reinterpret_cast < const uint8_t* > ( &_data_ ), sizeof ( T ) ); }

	template < typename T>
	uint8_t Read ( size_t _address_, T& _data_ )						{ return this -> ReadBlock ( _address_, reinterpret_cast < uint8_t* > ( &_data_ ), sizeof ( T ) ); }

	template < typename T>
	uint8_t Update ( size_t _address_, const T& _data_ ) {
		T _current_Value;
		this -> Read ( _address_, _current_Value );
		if ( _data_ != _current_Value ) return this -> Write ( _address_, _data_ );
		return 0;
	}
};

extern CLASS_I2CEEPROM_ eeprom_;
