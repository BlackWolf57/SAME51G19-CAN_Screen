#include "./monitor.h"
#include "../board/rtc.h"

CLASS_MONITOR_::CLASS_MONITOR_ () {
	this -> _VSSA = 0;
	this -> _now = 0;
	this -> _interval = 2;
}

uint16_t CLASS_MONITOR_::ADC_to_Power ( EAnalogChannel _channel_, uint16_t _R1_, uint16_t _R2_ ) {
	uint16_t _ADC = ADC_Value ( _channel_ ) + this -> _VSSA;
	float _divider = ( ( float ) ( _R1_ + _R2_ ) ) / ( ( float ) ( _R2_ ) );
	float _power = ( _ADC * ( _channel_ == MONITOR_VREF_CHANNEL ? 3300 : this -> voltage_.VRef () ) ) / 4095.0;

	return ( uint16_t ) ( _power * _divider );
}

uint8_t CLASS_MONITOR_::begin () {
	ADC_Init ();
	return 50;
}

void CLASS_MONITOR_::loop () {
	if ( time_.seconds > ( this -> _now + this -> _interval ) ) {
		this -> _VSSA = ADC_Value ( MONITOR_VSSA_CHANNEL );
		this -> voltage_.monitor_VSSA = ( uint16_t ) ( ( this -> _VSSA * UREF ) / 4095 );
		if ( ! this -> voltage_.State.bit.VSSA ) this -> voltage_.State.bit.VSSA = true;

		this -> voltage_.monitor_VRef = this -> ADC_to_Power ( MONITOR_VREF_CHANNEL, MONITORVREF_R1, MONITORVREF_R2 );
		if ( ! this -> voltage_.State.bit.VRef ) this -> voltage_.State.bit.VRef = true;

		this -> voltage_.monitor_VLogic = this -> ADC_to_Power ( MONITOR_3V3_CHANNEL, MONITOR3V3_R1, MONITOR3V3_R2 );
		if ( ! this -> voltage_.State.bit.VLogic ) this -> voltage_.State.bit.VLogic = true;

		this -> voltage_.monitor_VPower = this -> ADC_to_Power ( MONITOR_5V_CHANNEL, MONITOR5V_R1, MONITOR5V_R2 );
		if ( ! this -> voltage_.State.bit.VPower ) this -> voltage_.State.bit.VPower = true;

		this -> voltage_.monitor_VBatt = ( uint16_t ) ( ( ( ADC_Internal_Value ( ADC_INPUTCTRL_MUXPOS_SCALEDVBAT_Val ) + this -> _VSSA ) * ( UREF * 2 ) ) / 4095 );
		if ( ! this -> voltage_.State.bit.VBatt ) this -> voltage_.State.bit.VBatt = true;

		this -> voltage_.monitor_VCORE = ( uint16_t ) ( ( ( ADC_Internal_Value ( ADC_INPUTCTRL_MUXPOS_SCALEDCOREVCC_Val ) + this -> _VSSA ) * ( UREF * 2 ) ) / 4095 );
		if ( ! this -> voltage_.State.bit.VCORE ) this -> voltage_.State.bit.VCORE = true;

		this -> voltage_.monitor_IOVCC = ( uint16_t ) ( ( ( ADC_Internal_Value ( ADC_INPUTCTRL_MUXPOS_SCALEDIOVCC_Val ) + this -> _VSSA ) * ( UREF * 2 ) ) / 4095 );
		if ( ! this -> voltage_.State.bit.IOVCC ) this -> voltage_.State.bit.IOVCC = true;

		this -> _now = time_.seconds;
	}
}

CLASS_MONITOR_ monitor_;

