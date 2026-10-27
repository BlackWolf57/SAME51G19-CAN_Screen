#pragma once

#include "Arduino.h"

enum : uint8_t {
	month_January,
	month_February,
	month_March,
	month_April,
	month_May,
	month_June,
	month_July,
	month_August,
	month_September,
	month_October,
	month_November,
	month_December
} __month_enum__;

typedef enum : uint8_t {
	day_Sunday,
	day_Monday,
	day_Thuesday,
	day_Wednesday,
	day_Thirsday,
	day_Friday,
	day_Saturday
} day_t;

typedef enum : int8_t {
	hour_am = 0,
	hour_pm = 1,
	hour_24 = -1
} ampm_t;

typedef struct {
	typedef union {
		struct {
			uint8_t Initialized: 1;						// Le RTC est actif au niveau du µC.
			uint8_t Configured: 1;						// L'horloge du RTC as été entrée.
			uint8_t Synchronized: 1;					// L'horloge du RTC est récente.
			__I uint8_t :5;
		} bit;
		uint8_t reg;
	} rtc_state_t;

	rtc_state_t State;
	uint8_t SynchroDelta;								// Durée en heures entre 2 synchronisations de l'horloge
	int64_t LastSynchroTime;							// Horaire de la dernière synchronisation ( Format TimeStamp )
} rtc_state_t;

typedef struct {
	uint32_t seconds;
	uint32_t minutes () const							{ return ( this -> seconds < 60 ?    0 : ( uint32_t ) ( this -> seconds / 60 ) ); }		// 32 bits
	uint32_t hours () const								{ return ( this -> seconds < 3600 ?  0 : ( uint32_t ) ( this -> seconds / 3600 ) ); }		// 24 bits
	uint16_t days () const								{ return ( this -> seconds < 86400 ? 0 : ( uint32_t ) ( this -> seconds / 86400 ) ); }		// 16 bits
} custom_time_t;


class CLASS_RTC_ {
private:
	bool _winter_adjust;								// Drapeau pour utiliser l'ajustement pour l'heure été/hiver.
	int8_t _GMT;									// Pour faire correspondre à l'heure locale.
	int8_t _DayInputOffset;								// Offset de correction ( en jours ) pour que la conversion soit éxacte.
	int8_t _DayOutputOffset;							// Offset de correction ( en jours ) pour que la conversion soit éxacte.
	rtc_state_t state_;

	bool is_LeapYear ( uint16_t _Year_ )						{ return ( _Year_ % 4 == 0 && ( _Year_ % 100 != 0 || _Year_ % 400 == 0 ) ? true : false ); }
	bool is_WinterTime ();
	bool is_WinterTime ( const uint8_t _Month_, const uint8_t _Day_, const uint8_t _DayOfWeek_, const uint8_t _Hours_ );
	int64_t timeStamp ();
	int64_t timeStamp ( uint16_t _Year_, const uint8_t _Month_, const uint8_t _Day_ );

	void Disabled ();
	void Enabled ();
	void Reset ();
	void unReset ();

public:
	CLASS_RTC_ ();
	CLASS_RTC_ ( uint8_t _SynchroDelta_ );
	ampm_t am_pm () const								{ return ( ampm_t ) ( RTC -> MODE2.CTRLA.bit.CLKREP ? ( RTC -> MODE2.CLOCK.bit.HOUR & 0x10 ) >> 4 : hour_24 ); }
	uint16_t get_Year () const							{ return ( 2000 + RTC -> MODE2.CLOCK.bit.YEAR ); }
	uint8_t get_Month () const							{ return RTC -> MODE2.CLOCK.bit.MONTH; }
	uint8_t get_Day () const							{ return RTC -> MODE2.CLOCK.bit.DAY; }
	uint8_t get_Hour () const							{ return ( RTC -> MODE2.CTRLA.bit.CLKREP ? RTC -> MODE2.CLOCK.bit.HOUR & 0xF : RTC -> MODE2.CLOCK.bit.HOUR ); }
	uint8_t get_Minute () const							{ return RTC -> MODE2.CLOCK.bit.MINUTE; }
	uint8_t get_Second () const							{ return RTC -> MODE2.CLOCK.bit.SECOND; }
	day_t get_DayOfWeek ();
	day_t get_DayOfWeek ( int64_t _timeStamp_ ) const noexcept			{ return ( day_t ) ( _timeStamp_ >= -4 ? ( _timeStamp_ + 5 ) % 7 : ( ( _timeStamp_ + 6 ) % 7 ) + 6 ); }
	bool get_AutoWinter () const							{ return this -> _winter_adjust; }

	uint8_t begin ();
	void loop ();
	rtc_state_t RTC_State () const							{ return this -> state_; }
	void set_12hMode ();
	void set_24hMode ();
	void set_AutoWinter ( bool _adjust_ )						{ this -> _winter_adjust = _adjust_; }
	void set_GMT ( const int8_t _GMT_ )						{ this -> _GMT = _GMT_; }
	void set_InputOffset ( const int8_t _Offset_ )					{ this -> _DayInputOffset = _Offset_; }
	void set_OutputOffset ( const int8_t _Offset_ )					{ this -> _DayOutputOffset = _Offset_; }
	void set_SynchroDelta ( const uint8_t _SynchroDelta_ )				{ this -> state_.SynchroDelta = _SynchroDelta_; }
	void set_Offsets ( const int8_t _Input_Offset_, const int8_t _Output_Offset_ );
	void set_Clock ( const uint32_t _value_ );
	void set_TimeStamp ( uint64_t _timeStamp_ );
};

extern CLASS_RTC_ rtc_;
extern custom_time_t time_;

void RTC_Handler ();


