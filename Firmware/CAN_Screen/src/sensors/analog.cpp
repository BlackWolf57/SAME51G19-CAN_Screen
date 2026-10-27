#include "./analog.h"
#include "../board/clocks.h"

void ADC_Change_Ref ( uint8_t _ref_ ) {
	if ( ! ( ADC0 -> REFCTRL.bit.REFSEL == _ref_ ) ) {
		while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_REFCTRL );
		ADC0 -> REFCTRL.bit.REFSEL = _ref_;
		while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_REFCTRL );
	}
}

void ADC_Init () {
	static bool _initialized = false;

	if ( ! _initialized ) {
		uint32_t _biascomp = ( *( ( uint32_t *) ADC0_FUSES_BIASCOMP_ADDR ) & ADC0_FUSES_BIASCOMP_Msk ) >> ADC0_FUSES_BIASCOMP_Pos;
		uint32_t _biasr2r = ( *( ( uint32_t *) ADC0_FUSES_BIASR2R_ADDR ) & ADC0_FUSES_BIASR2R_Msk ) >> ADC0_FUSES_BIASR2R_Pos;
		uint32_t _biasref = ( *( ( uint32_t *) ADC0_FUSES_BIASREFBUF_ADDR ) & ADC0_FUSES_BIASREFBUF_Msk ) >> ADC0_FUSES_BIASREFBUF_Pos;

		while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE );
		ADC0 -> CTRLA.bit.ENABLE = 0;					// Désactivation de l'ADC n° 1
		while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE );

		SUPC -> VREF.reg = SUPC_VREF_SEL ( SUPC_VREF_SEL_1V0 ) |	// Sélection de la tension de référence.
				   SUPC_VREF_ONDEMAND |
				   SUPC_VREF_VREFOE |				// Envoi de la tension de référence vers l'ADC.
				   SUPC_VREF_TSEN;				// Activation du capteur de température interne.

		MCLK -> APBDMASK.reg |= MCLK_APBDMASK_ADC0;

		GCLK -> PCHCTRL [ ADC0_GCLK_ID ].reg = GCLK_PCHCTRL_GEN ( GENERIC_CLOCK_GENERATOR_48M ) |
						       GCLK_PCHCTRL_CHEN;

		ADC0 -> CTRLA.bit.PRESCALER = ADC_CTRLA_PRESCALER_DIV32_Val;	// ADC n° 1 fonctionnant à 1,5 MHz ( 48 MHz / 32 )

		ADC0 -> INPUTCTRL.bit.MUXNEG = ADC_INPUTCTRL_MUXNEG_GND_Val;
		while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_INPUTCTRL );

		ADC0 -> CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_12BIT_Val;
		while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_CTRLB );

		ADC_Change_Ref ( ADC_REFCTRL_REFSEL_INTVCC1_Val );
		//while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_REFCTRL );
		//ADC0 -> REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1_Val;
		//ADC0 -> REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTREF_Val;
		//while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_REFCTRL );

		ADC0 -> AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_1 |
				      ADC_AVGCTRL_ADJRES ( 0 );
		while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_AVGCTRL );

		ADC0 -> SAMPCTRL.reg = ADC_SAMPCTRL_SAMPLEN ( 7 );		// SAMPLEN = ( Sampling Time / CLKADC ) - 1 = ( > 10 µS / ( 48 MHz / 32 ) ) - 1 = minimum 14
		while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_SAMPCTRL );

		ADC0 -> CALIB.reg = ADC_CALIB_BIASREFBUF ( _biasref ) |
				    ADC_CALIB_BIASR2R ( _biasr2r ) |
				    ADC_CALIB_BIASCOMP ( _biascomp );

		_initialized = true;
	}
}

uint16_t ADC_Value ( EAnalogChannel _channel_ ) {
	if ( ( _channel_ >= ADC_Channel0 ) && ( _channel_ <= ADC_Channel15 ) ) {
		ADC_Change_Ref ( ADC_REFCTRL_REFSEL_INTVCC1_Val );
		//while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_REFCTRL );
		//ADC0 -> REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1_Val;
		//while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_REFCTRL );

		while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_INPUTCTRL );
		ADC0 -> INPUTCTRL.bit.MUXPOS = _channel_;

		while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE );
		ADC0 -> CTRLA.bit.ENABLE = 1;
		while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE );

		ADC0 -> SWTRIG.bit.START = 1;
		ADC0 -> INTFLAG.bit.RESRDY = 1;
		ADC0 -> SWTRIG.bit.START = 1;

		while ( ADC0 -> INTFLAG.bit.RESRDY == 0 );
		uint16_t _value = ADC0 -> RESULT.reg;

		while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE );
		ADC0 -> CTRLA.bit.ENABLE = 0;
		while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE );

		return _value;
	}

	return 0;
}

uint16_t ADC_Internal_Value ( uint8_t _channel_ ) {
	//ADC_Change_Ref ( ADC_REFCTRL_REFSEL_INTREF_Val );
	ADC_Change_Ref ( ADC_REFCTRL_REFSEL_INTVCC0_Val );
	//while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_REFCTRL );
	//ADC0 -> REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTREF_Val;
	//while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_REFCTRL );

	while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_INPUTCTRL );
	ADC0 -> INPUTCTRL.bit.MUXPOS = _channel_;

	while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE );
	ADC0 -> CTRLA.bit.ENABLE = 1;
	while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE );

	ADC0 -> SWTRIG.bit.START = 1;
	ADC0 -> INTFLAG.bit.RESRDY = 1;
	ADC0 -> SWTRIG.bit.START = 1;

	while ( ADC0 -> INTFLAG.bit.RESRDY == 0 );
	uint16_t _value = ADC0 -> RESULT.reg;

	while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE );
	ADC0 -> CTRLA.bit.ENABLE = 0;
	while ( ADC0 -> SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE );

	return _value;
}

