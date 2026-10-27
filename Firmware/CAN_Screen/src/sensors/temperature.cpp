#include "./temperature.h"
#include "./monitor.h"
#include "../board/clocks.h"
#include "../board/macros.h"
#include "../board/rtc.h"


CLASS_TEMPERATURE_::CLASS_TEMPERATURE_ () {
	this -> _now = 0;
	this -> _interval = 2;
	this -> temps_._type = deg_Celsius;
}

/** @brief			Enregistre la température interne du micro-contrôleur.
 *  @details
 */
void CLASS_TEMPERATURE_::internal () {
	while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_REFCTRL );
	ADC0 -> REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1_Val;
	while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_REFCTRL );

	while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_INPUTCTRL );
	ADC0 -> INPUTCTRL.bit.MUXPOS = ADC_INPUTCTRL_MUXPOS_PTAT_Val;

	while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE );
	ADC0 -> CTRLA.bit.ENABLE = 1;
	while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE );

	ADC0 -> SWTRIG.bit.START = 1;
	ADC0 -> INTFLAG.bit.RESRDY = 1;
	ADC0 -> SWTRIG.bit.START = 1;

	while ( ADC0 -> INTFLAG.bit.RESRDY == 0 );
	int16_t _TP = ADC0 -> RESULT.reg;

	while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_INPUTCTRL );
	ADC0 -> INPUTCTRL.bit.MUXPOS = ADC_INPUTCTRL_MUXPOS_CTAT_Val;

	ADC0 -> SWTRIG.bit.START = 1;
	ADC0 -> INTFLAG.bit.RESRDY = 1;
	ADC0 -> SWTRIG.bit.START = 1;

	ADC0 -> SWTRIG.bit.START = 1;
	ADC0 -> INTFLAG.bit.RESRDY = 1;
	ADC0 -> SWTRIG.bit.START = 1;

	while ( ADC0 -> INTFLAG.bit.RESRDY == 0 );
	int16_t _TC = ADC0 -> RESULT.reg;

	while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE );
	ADC0 -> CTRLA.bit.ENABLE = 0;
	while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE );

	if ( ( _TP > 0 ) && ( _TC > 0 ) ) {
		float _Temperature = ( ( TL * VPH * _TC ) - ( VPL * TH * _TC ) - ( TL * VCH * _TP ) + ( TH * VCL * _TP ) ) / ( ( VCL * _TP ) - ( VCH * _TP ) - ( VPL * _TC ) + ( VPH * _TC ) );
		_Temperature *= 100;
		this -> temps_.Chip = ( ( int16_t ) _Temperature ) & MASK_16BITS;
	}
	else this -> temps_.Chip = SENSOR_DISCONNECTED;							// 0x8000 = -32768 => -327,68 °C
}

/** @brief			Conversion d'une valeur brute en température.
 *  @details
 *  @param _adcValue_		Valeur ADC à convertir.
 */
int16_t CLASS_TEMPERATURE_::Thermistor ( const uint16_t _adcValue_ ) {
	float _Temperature = 0;
	uint16_t _ADC = _adcValue_ + monitor_.VSSA_ADC ();						// Prise en compte d'une éventuelle tension sur la masse.

	if ( ( _adcValue_ > 0 ) && ( _ADC < 4000 ) ) {
		_Temperature = log ( ( R1 / ( ( 4095.0 / _ADC ) - 1 ) ) / NTCVAL );
		_Temperature = _Temperature / NTCTYPE;
		_Temperature = _Temperature + ( 1 / ( 25 + 273.15 ) );
		_Temperature = ( 1 / _Temperature ) - 273.15;						// Conversion de °Kelvin en °Celsius.

		return ( int16_t ) ( _Temperature * 100 );
	}

	return SENSOR_DISCONNECTED;
}

/** @brief			Conversion d'une valeur brute en température.
 *  @details
 *  @param _adcValue_		Valeur ADC à convertir.
 * 				_adcValue_ environ 3145 avec alimentation +5V et capteur déconnecté !
 */
int16_t CLASS_TEMPERATURE_::LM335 ( const uint16_t _adcValue_ ) {
	int16_t _Temperature = SENSOR_DISCONNECTED;

	//if ( ( _adcValue_ > 0 ) && ( _adcValue_ < 3420 ) ) _Temperature = ( ( _adcValue_ * MATH_UREF_COEFF ) / R2 ) - 27315;
	if ( ( _adcValue_ > 0 ) && ( _adcValue_ < 3120 ) && ( monitor_.VPower () > 4000 ) ) _Temperature = ( ( _adcValue_ * COEFF_CONST ( monitor_.VRef () ) ) / R2 ) - 27315;
	return _Temperature;
}

uint8_t CLASS_TEMPERATURE_::begin () {
	ADC_Init ();
	return 75;
}

void CLASS_TEMPERATURE_::loop () {
	if ( time_.seconds > ( this -> _now + this -> _interval ) ) {
		this -> internal ();
		this -> temps_.Box = this -> LM335 ( ADC_Value ( BOX_SENSOR_CHANNEL ) );
		this -> temps_.Water = this -> Thermistor ( ADC_Value ( WATER_SENSOR_CHANNEL ) );

		this -> _now = time_.seconds;
	}
}

CLASS_TEMPERATURE_ temperatures_;

