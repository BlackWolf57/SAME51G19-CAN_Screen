#include "flow.h"
#include "../board/clocks.h"
#include "../board/events.h"
#include "../board/macros.h"
#include "../board/pins.h"
#include "../board/utils.h"

uint8_t CLASS_FLOW_::begin () {
	bool _config;
	uint8_t _port_pmux;
	uint32_t _position;
	uint32_t _extint= g_APinDescription [ FLOW_SENSOR ].ulExtInt;

	MCLK -> APBAMASK.reg |= MCLK_APBAMASK_TC0 | MCLK_APBAMASK_TC1 | MCLK_APBAMASK_EIC;
	MCLK -> APBBMASK.reg |= MCLK_APBBMASK_EVSYS;

	PORT -> Group [ g_APinDescription [ FLOW_SENSOR ].ulPort ].DIRCLR.reg = PORT_DIRCLR_DIRCLR ( 1 << g_APinDescription [ FLOW_SENSOR ].ulPin );
	PORT -> Group [ g_APinDescription [ FLOW_SENSOR ].ulPort ].PINCFG [ g_APinDescription [ FLOW_SENSOR ].ulPin ].bit.INEN = 1;
	PORT -> Group [ g_APinDescription [ FLOW_SENSOR ].ulPort ].PINCFG [ g_APinDescription [ FLOW_SENSOR ].ulPin ].bit.PMUXEN = 1;

	if ( is_pair ( g_APinDescription [ FLOW_SENSOR ].ulPin ) ) _port_pmux = PORT_PMUX_PMUXE ( 0 );
	else _port_pmux = PORT_PMUX_PMUXO ( 0 );
	PORT -> Group [ g_APinDescription [ FLOW_SENSOR ].ulPort ].PMUX [ g_APinDescription [ FLOW_SENSOR ].ulPin >> 1 ].reg = _port_pmux;

	GCLK -> PCHCTRL [ EIC_GCLK_ID ].reg = GCLK_PCHCTRL_GEN ( GENERIC_CLOCK_GENERATOR_1M6 ) |			// Utilisation de l'horloge à 1,6 MHz pour piloter l'EIC.
					      GCLK_PCHCTRL_CHEN;

	GCLK -> PCHCTRL [ TC0_GCLK_ID ].reg = GCLK_PCHCTRL_GEN ( GENERIC_CLOCK_GENERATOR_1M6 ) |			// Utilisation de l'horloge à 1,6 MHz pour piloter le timer.
					      GCLK_PCHCTRL_CHEN;

	EIC -> CTRLA.reg &= ~EIC_CTRLA_ENABLE;
	while ( EIC -> SYNCBUSY.bit.ENABLE );

	EVSYS -> USER [ EVSYS_ID_USER_TC0_EVU ].reg = EVSYS_USER_CHANNEL ( FLOW_EVENT_NB + 1 );				// Envoi de l'event user au timer TC0.

	if ( _extint > EXTERNAL_INT_7 ) {										// Copié depuis l'IDE Arduino !
		_config = 1;
		_position = ( _extint - 8 ) << 2;
	}
	else {
		_config = 0;
		_position = _extint << 2;
	}

	EIC -> EVCTRL.reg |= 1 << _extint;
	//EIC -> INTENSET.reg |= 1 << _extint;
	EIC -> CONFIG [ _config ].reg |= 2 << _position;								// Détection de front descendant !

	EVSYS -> Channel [ FLOW_EVENT_NB ].CHANNEL.reg = EVSYS_CHANNEL_EDGSEL ( EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT ) |		// Doit être à zéro en mode asynchrone !
							 EVSYS_CHANNEL_PATH ( EVSYS_CHANNEL_PATH_ASYNCHRONOUS ) |		// Affectation de l'interruption exterieure n°11 à un event user.
							 EVSYS_CHANNEL_EVGEN ( EVSYS_ID_GEN_EIC_EXTINT_11 );

	EIC -> CTRLA.reg |= EIC_CTRLA_ENABLE;
	while ( EIC -> SYNCBUSY.bit.ENABLE );

	TC0 -> COUNT32.CTRLA.reg = TC_CTRLA_CAPTEN0 |									// Activation de la capture dans CC1
				   TC_CTRLA_PRESCSYNC ( TC_CTRLA_PRESCSYNC_GCLK ) |					// Aucun prescaler
				   TC_CTRLA_MODE ( TC_CTRLA_MODE_COUNT32 );						// Compteur en mode 32 bits

	TC0 -> COUNT32.EVCTRL.reg = TC_EVCTRL_TCEI |									// Activation de l'event interne
				    //TC_EVCTRL_TCINV |									// Inversion de la polarité de l'event ( Comptage sur front descendant ! )
				    TC_EVCTRL_EVACT ( TC_EVCTRL_EVACT_STAMP );						// Mode capture TimeStamp

	TC0 -> COUNT32.CC [ 1 ].reg = 35000000;										// TOP value ?
	while ( TC0 -> COUNT32.SYNCBUSY.bit.CC1 );

	TC0 -> COUNT32.INTENSET.reg = TC_INTENSET_MC0;									// Interruption déclenchée par MC1 ( Valeur de TimeStamp )
	NVIC_SetPriority ( TC0_IRQn, 0 );
	NVIC_EnableIRQ ( TC0_IRQn );


	TC0 -> COUNT32.CTRLA.reg |= TC_CTRLA_ENABLE;									// Activation du compteur.
	while ( TC0 -> COUNT32.SYNCBUSY.bit.ENABLE );
	return 100;
}

CLASS_FLOW_ debimetre_;

void TC0_Handler ()							{ if ( TC0 -> COUNT32.INTFLAG.bit.MC0 ) debimetre_.set (); }
