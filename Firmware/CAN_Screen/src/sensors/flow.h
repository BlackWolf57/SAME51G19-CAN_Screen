#pragma once

#include "Arduino.h"
#include "../board/clocks.h"

#define PULSE_PER_LITER			169				// Nombre d'impulsions par litre
#define PULSE_PER_TRS			2				// Nombre d'impulsions par tours d'hélice
#define TIMER_FREQ			CLOCK_1M6_FREQ			// Fréquence de fonctionnement du timer
#define FLOW_DISCONNECTED		(int16_t) 0xFFFF		// Valeur par défaut ( -1 )

/* Formules de conversion:			Débit = 3600 / ( _timer_ x imp./l x ( 1 / Freq. timer ) )
 * 						Vitesse = ( Débit x imp./l ) / ( 60 x imp./trs. )
 * Valeurs en mode 'Compteur PW':		imp./l = 169,	Freq. timer = 100 kHz, timer 16 bits.
 * Valeurs en mode 'Capture Time-Stamp':	imp./l = 169,	Freq. timer = 1,6 MHz, timer 32 bits.		Minimum = 0x1AA0 pour 5000 l/h, Minimum = 0x2081018 pour 1 l/h
 */

typedef struct {
	int16_t Flow;
	int16_t Speed () const						{ return ( this -> Flow == FLOW_DISCONNECTED ? FLOW_DISCONNECTED : ( ( this -> Flow * PULSE_PER_LITER ) / ( 60 * PULSE_PER_TRS ) ) ); }

	//void Counter ( uint32_t _TC_Value_ )				{ this -> Flow = ( ( ( _TC_Value_ >= 0x1AA0 ) && ( _TC_Value_ <= 0x2081018 ) ) ? ( 3600 / ( _TC_Value_ * PULSE_PER_LITER * ( 1 / TIMER_FREQ ) ) ) : FLOW_DISCONNECTED ); }
	void Counter ( uint32_t _TC_Value_ )				{ this -> Flow = ( ( ( _TC_Value_ >= 0x1AA0 ) && ( _TC_Value_ <= 0x2081018 ) ) ? ( ( 3600ULL * TIMER_FREQ ) / ( _TC_Value_ * PULSE_PER_LITER ) ) : FLOW_DISCONNECTED ); }
} high_flow_t;								// sizeof () = 2

class CLASS_FLOW_ {
private:
	high_flow_t high_flow_;						// Taille = 2 bytes.

public:
	int16_t Flow () const						{ return this -> high_flow_.Flow; }
	int16_t Speed () const						{ return this -> high_flow_.Speed (); }
	void get ( high_flow_t *_buffer_ )				{ memcpy ( _buffer_, &this -> high_flow_, sizeof ( this -> high_flow_ ) ); }
	void set ()							{ this -> high_flow_.Counter ( TC0 -> COUNT32.CC [ 0 ].reg ); }
	void setDisconnected ()						{ this -> high_flow_.Flow = FLOW_DISCONNECTED; }
	uint8_t begin ();
	void sleep ();		//TODO - Stopper le compteur en mode veille
	void wakeup ();		//TODO - Redémarrer le compteur au réveil
};

extern CLASS_FLOW_ debimetre_;

void TC0_Handler ();
