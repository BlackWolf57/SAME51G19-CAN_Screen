#pragma once

#include "./analog.h"
#include "../board/utils.h"

#define SENSOR_DISCONNECTED		(int16_t) 0x8000		// -32768 d°C
#ifndef UREF
	#define UREF			3332				// Tension de référence ( en milliVolts ) pour les LM335.
#endif
#define R1				4700				// Valeur de la première résistance du pont diviseur pour les LM335.
#define R2				8200				// Valeur de la seconde résistance du pont diviseur pour les LM335.
#define MATH_UREF_COEFF			COEFF_CONST(UREF)		// Constante utilisé dans le calcul de température pour les LM335. Soulage le µC !
#define NTCVAL				10000				// Résistance de la sonde NTC à 25°C.
#define NTCTYPE				3950				// Type de résistance NTC.
#define COEFF_CONST(value)		((value*10*(R1+R2))/4095)	// ( 3332 x 10 x ( 4700 + 8200 ) ) / 4095


typedef enum : uint8_t {						// : uint8_t pour définir le poid en byte de l'enum !
	deg_Celsius = 0x1A,						// Températures en °C		( De -327,68°C à 327,67°C )
	deg_Kelvin = 0x07,						// Températures en °K		( De -54,53°K à 327,67°K )	( De -327,68°C à 54,52°C à cause de la limitation à 16 bits... )
	deg_Fahrenheit = 0x1B						// Températures en °F		( De -327,68°F à 327.67°F )	( De -199,82°C à 164,26°C à cause de la limitation à 16 bits... )
} degree_t;

typedef struct {
	degree_t _type;							// sizeof () = 1
	uint8_t Padding;						// sizeof () = 1

	int16_t Chip;							// sizeof () = 2
	int16_t Box;							// sizeof () = 2
	int16_t Water;							// sizeof () = 2

	int16_t Convert ( int16_t _temp_ )				{ return ( int16_t ) this -> Convert ( _temp_, this -> _type ); }
	int16_t Convert ( int16_t _temp_, degree_t _type_ ) {
		switch ( _type_ ) {
			case deg_Celsius:
				return ( int16_t ) _temp_;
				break;
			case deg_Kelvin:
				return ( int16_t ) ( _temp_ + 27315 );
				break;
			case deg_Fahrenheit:
				return ( int16_t ) ( ( ( ( float ) ( 9 / 5 ) ) * _temp_ ) + 32 );
				break;
			default:
				return ( int16_t ) SENSOR_DISCONNECTED;
				break;
		}
	}
	float Degree ( int16_t _temp_ )					{ return ( float ) this -> Degree ( _temp_, this -> _type ); }
	float Degree ( int16_t _temp_, degree_t _type_ ) {
		switch ( _type_ ) {
			case deg_Celsius:
				return ( float ) ( _temp_ / 100.0 );
				break;
			case deg_Kelvin:
				return ( float ) ( ( _temp_ + 27315 ) / 100.0 );
				break;
			case deg_Fahrenheit:
				return ( float ) ( ( ( 9 / 5 ) * ( _temp_ / 100.0 ) ) + 32 );
				break;
			default:
				return ( float ) ( SENSOR_DISCONNECTED / 100.0 );
				break;
		}
	}
} temperatures_t;							// sizeof () = 8

class CLASS_TEMPERATURE_ {
private:
	uint8_t _interval;
	uint32_t _now;

	temperatures_t temps_;						// Taille = 8 bytes.
	void internal ();
	int16_t LM335 ( const uint16_t _adcValue_ );
	int16_t Thermistor ( const uint16_t _adcValue_ );

public:
	//CLASS_TEMPERATURE_ ()						{ this -> _now = 0; }
	CLASS_TEMPERATURE_ ();
	uint32_t Interval () const					{ return this -> _interval * 1000; }
	float Chip ()							{ return this -> temps_.Degree ( this -> temps_.Chip ); }
	float Box ()							{ return this -> temps_.Degree ( this -> temps_.Box ); }
	float Water ()							{ return this -> temps_.Degree ( this -> temps_.Water ); }
	void get ( temperatures_t *_buffer_ )				{ memcpy ( _buffer_, &this -> temps_, sizeof ( this -> temps_ ) ); }
	uint8_t begin ();
	void loop ();
};

extern CLASS_TEMPERATURE_ temperatures_;

