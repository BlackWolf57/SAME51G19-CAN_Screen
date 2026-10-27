#include "./rtc.h"
#include "../screen/screen.h"

CLASS_RTC_::CLASS_RTC_ () {
	this -> _winter_adjust = true;
	this -> _GMT = 2;
	this -> _DayInputOffset = 0;
	this -> _DayOutputOffset = 0;
	this -> state_.State.reg = 0;
	this -> state_.SynchroDelta = 24;
}

CLASS_RTC_::CLASS_RTC_ ( uint8_t _SynchroDelta_ ) {
	this -> _winter_adjust = true;
	this -> _GMT = 2;
	this -> _DayInputOffset = 0;
	this -> _DayOutputOffset = 0;
	this -> state_.State.reg = 0;
	this -> state_.SynchroDelta = _SynchroDelta_;
}

/** @brief		Vérifie si l'horaire doit être ajusté en fonction de l'heure d'été et d'hiver.
 *  @details		TRUE = Heure d'hiver.			FALSE = Heure d'été.
 * 			Passage à l'heure d'été:   Dernier dimanche de mars, à 2h.   Avance d'une heure. ( 2h => 3h )
 * 			Passage à l'heure d'hiver: Dernier dimanche d'octobre, à 3h. Recul d'une heure.  ( 3h => 2h )
 */
bool CLASS_RTC_::is_WinterTime () {
	if ( this -> _winter_adjust ) {
		uint8_t _Month = this -> get_Month ();
		uint8_t _Day = this -> get_Day ();
		uint8_t _DayOfWeek = this -> get_DayOfWeek ();
		uint8_t _Hours = this -> get_Hour ();

		return this -> is_WinterTime ( _Month, _Day, _DayOfWeek, _Hours );
	}

	return false;
}

bool CLASS_RTC_::is_WinterTime ( const uint8_t _Month_, const uint8_t _Day_, const uint8_t _DayOfWeek_, const uint8_t _Hours_ ) {
	if ( this -> _winter_adjust ) {
		if ( ( _Month_ > month_March ) && ( _Month_ < month_October ) ) return false;
		if ( ( _Month_ < month_March ) || ( _Month_ > month_October ) ) return true;

		if ( _Month_ == month_March ) {
			if ( ( ( 31 - _Day_ ) + _DayOfWeek_ ) > 6 ) return true;
			else {
				if ( ( _DayOfWeek_ == day_Sunday ) && ( _Hours_ < 2 ) ) return true;
				else return false;
			}
		}
		if ( _Month_ == month_October ) {
			if ( ( ( 31 - _Day_ ) + _DayOfWeek_ ) > 6 ) return false;
			else {
				if ( ( _DayOfWeek_ == day_Sunday ) && ( _Hours_ >= 3 ) ) return true;
				else return false;
			}
		}
	}

	return false;
}

int64_t CLASS_RTC_::timeStamp () {
	const uint16_t _Year = this -> get_Year ();
	const uint8_t _Month = this -> get_Month ();
	const uint8_t _Day = this -> get_Day ();

	return this -> timeStamp ( _Year, _Month, _Day );
}

int64_t CLASS_RTC_::timeStamp ( uint16_t _Year_, const uint8_t _Month_, const uint8_t _Day_ ) {
	_Year_ -= _Month_ <= 2;
	const uint8_t _ERA = ( _Year_ >= 0 ? _Year_ : _Year_ - 399 ) / 400;
	const uint16_t _YearOfERA = _Year_ - ( _ERA * 400 );
	const uint16_t _DayOfYear = ( ( uint16_t ) ( ( ( 153 * ( _Month_ > 2 ? _Month_ - 3 : _Month_ + 9 ) ) + 2 ) / 5 ) ) + ( _Day_ - 1 );
	const uint32_t _DayOfERA = ( _YearOfERA * 365 ) + ( ( uint8_t ) ( _YearOfERA / 4 ) ) - ( ( uint8_t ) ( _YearOfERA / 100 ) ) + _DayOfYear;

	return ( ( _ERA * 146097 ) + ( _DayOfERA - ( 719468 + this -> _DayOutputOffset ) ) );
}

void CLASS_RTC_::Disabled () {
	RTC -> MODE2.CTRLA.reg &= ~RTC_MODE2_CTRLA_ENABLE;
	while ( RTC -> MODE2.SYNCBUSY.reg & RTC_MODE2_SYNCBUSY_ENABLE );
	this -> state_.State.bit.Initialized = false;
}

void CLASS_RTC_::Enabled () {
	RTC -> MODE2.CTRLA.reg |= RTC_MODE2_CTRLA_ENABLE;
	while ( RTC -> MODE2.SYNCBUSY.reg & RTC_MODE2_SYNCBUSY_ENABLE );
	this -> state_.State.bit.Initialized = true;
}

void CLASS_RTC_::Reset () {
	RTC -> MODE2.CTRLA.reg |= RTC_MODE2_CTRLA_SWRST;
	while ( RTC -> MODE2.SYNCBUSY.reg & RTC_MODE2_SYNCBUSY_SWRST );
}

void CLASS_RTC_::unReset () {
	RTC -> MODE2.CTRLA.reg &= ~RTC_MODE2_CTRLA_SWRST;
	while ( RTC -> MODE2.SYNCBUSY.reg & RTC_MODE2_SYNCBUSY_SWRST );
}

/** @brief		Retourne le jour de la semaine
 *  @details		0 = Dimanche, 1 = Lundi, etc...
 */
day_t CLASS_RTC_::get_DayOfWeek () {
	int64_t _timeStamp = this -> timeStamp ();
	return this -> get_DayOfWeek ( _timeStamp );
}

/** @brief
 *  @details
 * 			INFO: RTC -> MODE2 et RTC -> MODE2 partagent les mêmes adresses mémoire. Seul leur fonctionnement diffère.
 * 			Pour les interruptions et les événements le code est marqué comme RTC -> MODE2, mais ça fonctionne aussi pour RTC -> MODE2 !
 */
uint8_t CLASS_RTC_::begin () {
	OSC32KCTRL -> RTCCTRL.bit.RTCSEL = OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K;					// Utilisation de la sortie 1,024kHz de l'oscillateur interne 32,768kHz ULP comme source pour le RTC.
	MCLK -> APBAMASK.reg |= MCLK_APBAMASK_OSC32KCTRL | MCLK_APBAMASK_RTC;

	if ( ( RSTC -> RCAUSE.bit.BACKUP ) && ( RSTC -> BKUPEXIT.bit.BBPS ) ) this -> state_.State.bit.Configured = true;
	else {
		this -> Disabled ();
		this -> Reset ();

		RTC -> MODE2.CTRLA.reg = RTC_MODE2_CTRLA_PRESCALER_DIV1024 |					// Prescaler de 1024, car horloge à 1,024kHz. 1,024kHz / 1024 = 1Hz !
					 RTC_MODE2_CTRLA_CLOCKSYNC |
					 //RTC_MODE2_CTRLA_CLKREP |						// Horloge de 0 à 12.
					 RTC_MODE2_CTRLA_MODE_CLOCK;						// Mode Horloge humaine
	}

	RTC -> MODE2.CTRLB.reg = RTC_MODE2_CTRLB_GP2EN |
				 RTC_MODE2_CTRLB_GP0EN;

	RTC -> MODE2.INTENSET.reg = RTC_MODE2_INTENSET_PER7 |							// Interruption toutes les secondes.		( 1 s )
				    RTC_MODE2_INTENSET_PER6;							// Interruption toutes les demi-secondes.	( 0,5 s )

	NVIC_SetPriority ( RTC_IRQn, 2 );
	NVIC_EnableIRQ ( RTC_IRQn );

	this -> Enabled ();
	return 25;
}

void CLASS_RTC_::loop () {
	if ( this -> state_.State.bit.Synchronized ) {
		uint32_t _synchro = this -> state_.LastSynchroTime + ( this -> state_.SynchroDelta * 3600 );
		this -> state_.State.bit.Synchronized = ( this -> timeStamp () < _synchro );
	}
}

void CLASS_RTC_::set_12hMode () {
	this -> Disabled ();
	RTC -> MODE2.CTRLA.reg |= RTC_MODE2_CTRLA_CLKREP;
	this -> Enabled ();
}

void CLASS_RTC_::set_24hMode () {
	this -> Disabled ();
	RTC -> MODE2.CTRLA.reg &= ~RTC_MODE2_CTRLA_CLKREP;
	this -> Enabled ();
}

void CLASS_RTC_::set_Offsets ( const int8_t _Input_Offset_, const int8_t _Output_Offset_ ) {
	this -> _DayInputOffset = _Input_Offset_;
	this -> _DayOutputOffset = _Output_Offset_;
}

void CLASS_RTC_::set_Clock ( const uint32_t _value_ ) {
	RTC -> MODE2.CLOCK.reg = _value_;
	while ( RTC -> MODE2.SYNCBUSY.reg & RTC_MODE2_SYNCBUSY_CLOCK );

	this -> state_.State.bit.Configured = true;
	this -> state_.State.bit.Synchronized = true;
	this -> state_.LastSynchroTime = this -> timeStamp ();
	this -> unReset ();
}

void CLASS_RTC_::set_TimeStamp ( uint64_t _timeStamp_ ) {
	_timeStamp_ += ( this -> _GMT * 3600 );									// Ajout du décallage horaire !
	uint32_t _timePart = _timeStamp_ % 86400;								// La partie contenant l'heure.		( Unité: Secondes. )
	uint64_t _datePart = ( _timeStamp_ - _timePart ) / 86400;						// La partie contenant la date.		( Unité: Jours. )

	uint8_t _Seconds = _timePart % 60;
	_timePart /= 60;
	uint8_t _Minutes = _timePart % 60;
	_timePart /= 60;
	uint8_t _Hours = _timePart % 24;

	/** @details		Voir l'URL ci-dessous pour plus d'informations sur le calcul.
	 *  @URL		http://howardhinnant.github.io/date_algorithms.html
	 */
	_datePart += ( 719468 + this -> _DayInputOffset );
	const int8_t _ERA = ( ( _datePart >= 0 ? _datePart : _datePart - 146096 ) / 146097 );
	const uint32_t _DayOfERA = _datePart - ( _ERA * 146097 );
	const uint16_t _YearOfERA = ( _DayOfERA - ( ( uint8_t ) ( _DayOfERA / 1460 ) ) + ( ( uint8_t ) ( _DayOfERA / 36524 ) ) - ( ( uint8_t ) ( _DayOfERA / 146096 ) ) ) / 365;
	const uint16_t _DayOfYear = _DayOfERA - ( ( 365 * _YearOfERA ) + ( ( uint8_t ) ( _YearOfERA / 4 ) ) - ( ( uint8_t ) ( _YearOfERA / 100 ) ) );
	const uint8_t _ShiftedMonth = ( ( 5 * _DayOfYear ) + 2 ) / 153;
	uint8_t _Day = _DayOfYear - ( ( uint16_t ) ( ( ( 153 * _ShiftedMonth ) + 2 ) / 5 ) ) + 1;
	uint8_t _Month = ( _ShiftedMonth < 10 ? _ShiftedMonth + 3 : _ShiftedMonth - 9 );
	uint16_t _Year = ( uint16_t ) ( _YearOfERA + ( _ERA * 400 ) + ( _Month <= 2 ) );

	RTC -> MODE2.CLOCK.reg = RTC_MODE2_CLOCK_SECOND ( _Seconds ) |
				 RTC_MODE2_CLOCK_MINUTE ( _Minutes ) |
				 RTC_MODE2_CLOCK_HOUR ( this -> is_WinterTime ( _Month, _Day, this -> get_DayOfWeek ( _timeStamp_ ), _Hours ) ? _Hours - 1 : _Hours ) |
				 RTC_MODE2_CLOCK_DAY ( _Day ) |
				 RTC_MODE2_CLOCK_MONTH ( _Month ) |
				 RTC_MODE2_CLOCK_YEAR ( _Year - 2000 );
	while ( RTC -> MODE2.SYNCBUSY.reg & RTC_MODE2_SYNCBUSY_CLOCK );

	this -> state_.State.bit.Configured = true;
	this -> state_.State.bit.Synchronized = true;
	this -> state_.LastSynchroTime = _timeStamp_;
	this -> unReset ();
}

CLASS_RTC_ rtc_;
custom_time_t time_;

void RTC_Handler () {
	if ( RTC -> MODE2.INTFLAG.bit.PER7 ) {
		time_.seconds ++;									// Incrémentation du compteur de secondes
		RTC -> MODE2.INTFLAG.bit.PER7 = 1;
	}

	if ( RTC -> MODE2.INTFLAG.bit.PER6 ) {
		screen_.refresh_HalfSecond ();
		RTC -> MODE2.INTFLAG.bit.PER6 = 1;
	}
}

