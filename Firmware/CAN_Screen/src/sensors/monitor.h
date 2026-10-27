#pragma once

#include "Arduino.h"
#include "./analog.h"
#include "../board/macros.h"

#ifndef UREF
	#define UREF			3332				// Tension de référence ( en milliVolts )
#endif
#define MONITOR3V3_R1			10000				// Valeur de la première résistance du pont diviseur pour le suivi du +3,3V
#define MONITOR3V3_R2			10000				// Valeur de la seconde résistance du pont diviseur pour le suivi du +3,3V
#define MONITOR5V_R1			22000				// Valeur de la première résistance du pont diviseur pour le suivi du +5V
#define MONITOR5V_R2			10000				// Valeur de la seconde résistance du pont diviseur pour le suivi du +5V
#define MONITORVREF_R1			27				// Valeur de la première résistance du pont diviseur pour le suivi de la tension de référence
#define MONITORVREF_R2			4700				// Valeur de la seconde résistance du pont diviseur pour le suivi de la tension de référence


typedef struct {
	typedef union {
		struct {
			uint8_t VCORE: 1;
			uint8_t VBatt: 1;
			uint8_t IOVCC: 1;
			__I uint8_t : 1;
			uint8_t VRef: 1;
			uint8_t VPower: 1;
			uint8_t VLogic: 1;
			uint8_t VSSA: 1;
		} bit;
		uint8_t Updated;
	} state_t;							// sizeof () = 1

	state_t State;							// Etat pour savoir si tensions encore à zéro ou non.
	uint8_t Padding;
	uint16_t monitor_VCORE;						// Tension de coeur.			Environ 1200 mV
	uint16_t monitor_VBatt;						// Tension de la pile RTC.		Environ 3000 mV
	uint16_t monitor_IOVCC;						// Tension des broches I/O.		Environ 3300 mV ( VLogic )
	uint16_t monitor_VRef;						// Tension de référence.
	uint16_t monitor_VPower;					// Tension 5V.				Environ 5000 mV
	uint16_t monitor_VLogic;					// Tension 3,3V.			Environ 3300 mV
	uint16_t monitor_VSSA;						// Tension sur la masse.		Environ 0 mV ?

	bool Updated () const						{ return ( this -> State.Updated == 0xF7 ); }
	uint16_t VCORE () const						{ return ( this -> State.bit.VCORE ? this -> monitor_VCORE : 1200 ); }
	uint16_t VBatt () const						{ return ( this -> State.bit.VBatt ? this -> monitor_VBatt : 0 ); }
	uint16_t IOVCC () const						{ return ( this -> State.bit.IOVCC ? this -> monitor_IOVCC : 3300 ); }
	uint16_t VRef () const						{ return ( this -> State.bit.VRef ? this -> monitor_VRef : 3300 ); }
	uint16_t VPower () const					{ return ( this -> State.bit.VPower ? this -> monitor_VPower : 5000 ); }
	uint16_t VLogic () const					{ return ( this -> State.bit.VLogic ? this -> monitor_VLogic : 3300 ); }
	uint16_t VSSA () const						{ return ( this -> State.bit.VSSA ? this -> monitor_VSSA : 0 ); }
} voltage_t;								// sizeof () = 16

class CLASS_MONITOR_ {
private:
	uint8_t _interval;
	uint32_t _now;
	uint16_t _VSSA;
	uint16_t ADC_to_Power ( EAnalogChannel _channel_, uint16_t _R1_, uint16_t _R2_ );

	voltage_t voltage_;						// Taille = 16 bytes.

public:
	CLASS_MONITOR_ ();
	uint32_t Interval () const					{ return this -> _interval * 1000; }
	uint16_t VSSA_ADC () const					{ return this -> _VSSA; }
	uint16_t VSSA () const						{ return this -> voltage_.VSSA (); }
	uint16_t VLogic () const					{ return this -> voltage_.VLogic (); }
	uint16_t VPower () const					{ return this -> voltage_.VPower (); }
	uint16_t VRef () const						{ return this -> voltage_.VRef (); }
	uint16_t IOVCC () const						{ return this -> voltage_.IOVCC (); }
	uint16_t VBatt () const						{ return this -> voltage_.VBatt (); }
	uint16_t VCORE () const						{ return this -> voltage_.VCORE (); }
	void get ( voltage_t *_buffer_ )				{ memcpy ( _buffer_, &this -> voltage_, sizeof ( this -> voltage_ ) ); }
	uint8_t begin ();
	void loop ();
};

extern CLASS_MONITOR_ monitor_;

