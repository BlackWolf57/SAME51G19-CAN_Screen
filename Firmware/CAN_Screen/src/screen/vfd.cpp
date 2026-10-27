#include "./vfd.h"
#include "../board/pins.h"
#include "../sensors/monitor.h"

CLASS_VFD_::CLASS_VFD_ () {
	this -> _data_pins [ 0 ] = SCREEN_DB0;				// PA.00
	this -> _data_pins [ 1 ] = SCREEN_DB1;				// PA.01
	this -> _data_pins [ 2 ] = SCREEN_DB2;				// PA.05
	this -> _data_pins [ 3 ] = SCREEN_DB3;				// PA.04
	this -> _data_pins [ 4 ] = SCREEN_DB4;				// PB.23
	this -> _data_pins [ 5 ] = SCREEN_DB5;				// PB.22
	this -> _data_pins [ 6 ] = SCREEN_DB6;				// PA.18
	this -> _data_pins [ 7 ] = SCREEN_DB7;				// PA.19

	this -> displayFunction_.bit.Command = 1;
	this -> displayControl_.bit.Command = 1;
	this -> displayEntry_.bit.Command = 1;
	this -> displayShift_.bit.Command = 1;

	this -> displayFunction_.bit.Lines = BIT_2LINE;
	this -> displayFunction_.bit.Data = BIT_DATA_8BITS;

	this -> displayControl_.bit.Blink = false;
	this -> displayControl_.bit.Cursor = false;
	this -> displayControl_.bit.Display = true;					//TODO: Faire initialisation depuis ROM

	this -> displayEntry_.bit.Shift = BIT_CURSOR_SHIFT_EN;
	this -> displayEntry_.bit.Direction = BIT_SHIFT_TO_RIGHT;

	this -> displayShift_.bit.Direction = BIT_SHIFT_TO_RIGHT;
	this -> displayShift_.bit.Type = BIT_CURSOR_SHIFT;
}

void CLASS_VFD_::begin ( vfd_brightness_t _brightness_ ) {
	MCLK -> APBBMASK.reg |= MCLK_APBBMASK_PORT;

	pinMode ( SCREEN_RESET, OUTPUT );
	pinMode ( SCREEN_RS, OUTPUT );
	pinMode ( SCREEN_RW, OUTPUT );
	pinMode ( SCREEN_EN, OUTPUT );
	for ( uint8_t _i = 0; _i < 8; _i ++ ) {
		pinMode ( this -> _data_pins [ _i ], OUTPUT );
	}

	digitalWrite ( SCREEN_RESET, HIGH );
	digitalWrite ( SCREEN_RS, LOW );
	digitalWrite ( SCREEN_RW, LOW );
	digitalWrite ( SCREEN_EN, LOW );
	delayMicroseconds ( 100 );

	this -> command ( this -> displayEntry_.reg );									// Configuration de l'affichage ( Aligné à gauche )
	this -> setBrightness ( _brightness_ );
	this -> clear ();												// Effacement de l'affichage
}

void CLASS_VFD_::setBrightness ( vfd_brightness_t _brightness_ ) {
	this -> noDisplay ();												// Arrêt de l'affichage nécessaire pour modification de la luminausité !
	this -> displayFunction_.bit.Brightness = ( uint8_t ) ( _brightness_ & 0x3 );					// Définition de l'intensité lumineuse
	this -> command ( this -> displayFunction_.reg );								// Configuration de l'afficheur ( 8 bits, 2 lignes et luminausité )
	this -> display ();												// Après définition, remise en route de l'affichage !
}

void CLASS_VFD_::reset () {
	digitalWrite ( SCREEN_RESET, LOW );
	delayMicroseconds ( 50 );
	digitalWrite ( SCREEN_RESET, HIGH );
}

void CLASS_VFD_::noDisplay () {
	this -> displayControl_.bit.Display = false;
	this -> command ( this -> displayControl_.reg );
}

void CLASS_VFD_::display () {
	this -> displayControl_.bit.Display = true;
	this -> command ( this -> displayControl_.reg );
}

void CLASS_VFD_::noCursor () {
	this -> displayControl_.bit.Cursor = false;
	this -> command ( this -> displayControl_.reg );
}

void CLASS_VFD_::cursor () {
	this -> displayControl_.bit.Cursor = true;
	this -> command ( this -> displayControl_.reg );
}

void CLASS_VFD_::noBlink () {
	this -> displayControl_.bit.Blink = false;
	this -> command ( this -> displayControl_.reg );
}

void CLASS_VFD_::blink () {
	this -> displayControl_.bit.Blink = true;
	this -> command ( this -> displayControl_.reg );
}

void CLASS_VFD_::scrollDisplayLeft () {
	this -> displayShift_.bit.Type = BIT_DISPLAY_SHIFT;
	this -> displayShift_.bit.Direction = BIT_SHIFT_TO_LEFT;
	this -> command ( this -> displayShift_.reg );
}

void CLASS_VFD_::scrollDisplayRight () {
	this -> displayShift_.bit.Type = BIT_DISPLAY_SHIFT;
	this -> displayShift_.bit.Direction = BIT_SHIFT_TO_RIGHT;
	this -> command ( this -> displayShift_.reg );
}

void CLASS_VFD_::scrollCursorLeft () {
	this -> displayShift_.bit.Type = BIT_CURSOR_SHIFT;
	this -> displayShift_.bit.Direction = BIT_SHIFT_TO_LEFT;
	this -> command ( this -> displayShift_.reg );
}

void CLASS_VFD_::scrollCursorRight () {
	this -> displayShift_.bit.Type = BIT_CURSOR_SHIFT;
	this -> displayShift_.bit.Direction = BIT_SHIFT_TO_RIGHT;
	this -> command ( this -> displayShift_.reg );
}

void CLASS_VFD_::leftToRight () {
	this -> displayEntry_.bit.Direction = BIT_SHIFT_TO_RIGHT;
	this -> command ( this -> displayEntry_.reg );
}

void CLASS_VFD_::rightToLeft () {
	this -> displayEntry_.bit.Direction = BIT_SHIFT_TO_LEFT;
	this -> command ( this -> displayEntry_.reg );
}

void CLASS_VFD_::autoScroll () {
	this -> displayEntry_.bit.Shift = BIT_DISPLAY_SHIFT_EN;
	this -> command ( this -> displayEntry_.reg );
}

void CLASS_VFD_::noAutoScroll () {
	this -> displayEntry_.bit.Shift = BIT_CURSOR_SHIFT_EN;
	this -> command ( this -> displayEntry_.reg );
}

void CLASS_VFD_::createChar ( uint8_t _location_, uint8_t _charmap_ [] ) {
	_location_ &= 0x07;						// Only 8 locations ( 0-7 )
	this -> command ( VFD_SETCGRAMADDR | ( _location_ << 3 ) );
	for ( uint8_t _i = 0; _i < 8; _i ++ ) {
		this -> write ( _charmap_ [ _i ] );
	}
}

inline size_t CLASS_VFD_::write ( uint8_t _value_ ) {
	this -> send ( _value_, VFD_WRITE );
	return 1;
}

size_t CLASS_VFD_::writeOn ( uint8_t _value_, uint8_t _pos_ ) {
	this -> setCursor ( _pos_ );
	return this -> write ( _value_ );
}

size_t CLASS_VFD_::writeOn ( const uint8_t *_buffer_, size_t _size_, uint8_t _pos_ ) {
	this -> setCursor ( _pos_ );
	return this -> write ( _buffer_, _size_ );
}

void CLASS_VFD_::send ( uint8_t _value_, vfd_rs_mode_t _mode_ ) {
	if ( monitor_.VPower () > 4000 ) {					// Pour être certain que l'écran est bien alimenté !
		digitalWrite ( SCREEN_RS, ( bool ) _mode_ );
		digitalWrite ( SCREEN_RW, LOW );
		digitalWrite ( SCREEN_EN, HIGH );

		for ( uint8_t _i = 0; _i < 8; _i ++ ) {
			digitalWrite ( this -> _data_pins [ _i ], ( _value_ >> _i ) & 0x01 );
		}

		digitalWrite ( SCREEN_EN, LOW );
		delayMicroseconds ( 50 );					// Commands need > 37µS to settle
	}
}

CLASS_VFD_ vfd_;

