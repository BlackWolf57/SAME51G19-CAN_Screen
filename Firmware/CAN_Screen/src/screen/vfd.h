#pragma once

#include "Arduino.h"
#include "Print.h"

// Commandes
#define VFD_CLEARDISPLAY				0x01
#define VFD_RETURNHOME					0x02
#define VFD_SETCGRAMADDR				0x40
#define VFD_SETDDRAMADDR				0x80

typedef enum {
	VFD_Brightness100 = 0,
	VFD_Brightness75 = 1,
	VFD_Brightness50 = 2,
	VFD_Brightness25 = 3
} vfd_brightness_t;
/*typedef union {
	struct {
		uint8_t BR0: 1;
		uint8_t BR1: 1;
		uint8_t : 6;
	} bit;
	uint8_t Brightness: 2;
	uint8_t : 6;
}*/

class CLASS_VFD_ : public Print {
private:
	typedef enum {
		VFD_COMMAND = LOW,
		VFD_WRITE = HIGH
	} vfd_rs_mode_t;
	typedef enum {
		// Entry mode set
		BIT_INCREMENT = true,
		BIT_DECREMENT = false,
		BIT_DISPLAY_SHIFT_EN = true,
		BIT_CURSOR_SHIFT_EN = false,

		// Cursor or display shift
		BIT_DISPLAY_SHIFT = true,
		BIT_CURSOR_SHIFT = false,
		BIT_SHIFT_TO_RIGHT = true,
		BIT_SHIFT_TO_LEFT = false,

		// Function set
		BIT_DATA_8BITS = true,
		BIT_DATA_4BITS = false,
		BIT_2LINE = true,
		BIT_1LINE = false
	} vfd_bit_t;
	typedef union {
		struct {
			uint8_t Blink: 1;
			uint8_t Cursor: 1;
			uint8_t Display: 1;
			uint8_t Command: 1;
			__I uint8_t :4;
		} bit;
		uint8_t reg;
	} vfd_control_t;
	typedef union {
		struct {
			uint8_t Shift: 1;
			uint8_t Direction: 1;
			uint8_t Command: 1;
			__I uint8_t : 5;
		} bit;
		uint8_t reg;
	} vfd_entry_t;
	typedef union {
		struct {
			uint8_t : 2;
			uint8_t Direction: 1;
			uint8_t Type: 1;
			uint8_t Command: 1;
			__I uint8_t : 3;
		} bit;
		uint8_t reg;
	} vfd_shift_t;
	typedef union {
		struct {
			uint8_t Brightness: 2;
			uint8_t : 1;
			uint8_t Lines: 1;
			uint8_t Data: 1;
			uint8_t Command: 1;
			__I uint8_t : 2;
		} bit;
		uint8_t reg;
	} vfd_function_t;

	uint8_t _data_pins [ 8 ];

	vfd_control_t displayControl_;
	vfd_entry_t displayEntry_;
	vfd_shift_t displayShift_;
	vfd_function_t displayFunction_;

	void home ()								{ this -> command ( VFD_RETURNHOME ); }
	void clear ()								{ this -> command ( VFD_CLEARDISPLAY ); }
	void setCursor ( uint8_t _pos_ )					{ this -> command ( VFD_SETDDRAMADDR | ( _pos_ < 0x40 ? _pos_ % 40 : ( ( _pos_ % 0x40 ) % 40 ) | 0x40 ) ); }
	void setCursor ( uint8_t _col_, uint8_t _row_ )				{ this -> command ( VFD_SETDDRAMADDR | ( ( _col_ % 40 ) + ( ( _row_ % 2 ) * 0x40 ) ) ); }
	void noDisplay ();
	void display ();
	void noCursor ();
	void cursor ();
	void noBlink ();
	void blink ();
	void scrollDisplayLeft ();
	void scrollDisplayRight ();
	void scrollCursorLeft ();
	void scrollCursorRight ();
	void leftToRight ();
	void rightToLeft ();
	void autoScroll ();
	void noAutoScroll ();
	bool wakeup ();
	void createChar ( uint8_t _location_, uint8_t _charmap_ [] );
	inline void command ( uint8_t _value_ )					{ this -> send ( _value_, VFD_COMMAND ); }
	size_t writeOn ( uint8_t _value_, uint8_t _pos_ );
	size_t writeOn ( const uint8_t *_buffer_, size_t _size_, uint8_t _pos_ );
	void send ( uint8_t _value_, vfd_rs_mode_t _mode_ );

	friend class CLASS_SCREEN_;

public:
	CLASS_VFD_ ();
	void begin ( vfd_brightness_t _brightness_ = VFD_Brightness100 );
	void setBrightness ( vfd_brightness_t _brightness_ = VFD_Brightness100 );
	void reset ();
	virtual size_t write ( uint8_t _value_ );

	using Print::write;
};

extern CLASS_VFD_ vfd_;
