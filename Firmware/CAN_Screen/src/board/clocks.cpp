#include "./clocks.h"
#include "./macros.h"

/*void InitClocks () {
	NVMCTRL -> CTRLA.reg |= NVMCTRL_CTRLA_RWS ( 0 );

	OSC32KCTRL -> OSCULP32K.reg |= OSC32KCTRL_OSCULP32K_EN1K | OSC32KCTRL_OSCULP32K_EN32K;				// Configuration de l'oscillateur interne de 32,768kHz
	GCLK -> CTRLA.bit.SWRST = 1;											// Reset logiciel !
	while ( GCLK -> SYNCBUSY.reg & GCLK_SYNCBUSY_SWRST );

	// Utilisation de l'oscillateur interne comme source pour "Generic Clock Generator 0", alias "MAIN_CLOCK_SOURCE"
	GCLK -> GENCTRL [ GENERIC_CLOCK_GENERATOR_MAIN ].reg = GCLK_GENCTRL_SRC ( GCLK_GENCTRL_SRC_OSCULP32K ) |
							       GCLK_GENCTRL_GENEN;
	while ( GCLK -> SYNCBUSY.reg & GCLK_SYNCBUSY ( GENERIC_CLOCK_GENERATOR_MAIN ) );

	// Configuration du DFLL.
	OSCCTRL -> DFLLCTRLA.reg = 0;

	OSCCTRL -> DFLLMUL.reg = OSCCTRL_DFLLMUL_CSTEP ( 0x01 ) |
				 OSCCTRL_DFLLMUL_FSTEP ( 0x01 ) |
				 OSCCTRL_DFLLMUL_MUL ( 0 );
	while ( OSCCTRL -> DFLLSYNC.reg & OSCCTRL_DFLLSYNC_DFLLMUL );

	OSCCTRL -> DFLLCTRLB.reg = 0;
	while ( OSCCTRL -> DFLLSYNC.reg & OSCCTRL_DFLLSYNC_DFLLCTRLB );

	OSCCTRL -> DFLLCTRLA.reg |= OSCCTRL_DFLLCTRLA_ENABLE;
	while ( OSCCTRL -> DFLLSYNC.reg & OSCCTRL_DFLLSYNC_ENABLE );

	OSCCTRL -> DFLLVAL.reg = OSCCTRL -> DFLLVAL.reg;
	while ( OSCCTRL -> DFLLSYNC.bit.DFLLVAL );

	OSCCTRL -> DFLLCTRLB.reg = OSCCTRL_DFLLCTRLB_WAITLOCK |
				   OSCCTRL_DFLLCTRLB_CCDIS |
				   OSCCTRL_DFLLCTRLB_USBCRM;
	while ( !OSCCTRL -> STATUS.bit.DFLLRDY );

	// Passage de l'oscillateur interne au DFLL à 48 MHz pour "Generic Clock Generator 0", alias "MAIN_CLOCK_SOURCE"
	//GCLK -> GENCTRL [ GENERIC_CLOCK_GENERATOR_MAIN ].reg = GCLK_GENCTRL_SRC ( GCLK_GENCTRL_SRC_DFLL ) |
	//						       GCLK_GENCTRL_IDC |
	//						       //GCLK_GENCTRL_OE |
	//						       GCLK_GENCTRL_GENEN;
	//while ( GCLK -> SYNCBUSY.reg & GCLK_SYNCBUSY ( GENERIC_CLOCK_GENERATOR_MAIN ) );

	// Configuration de "Generic Clock Generator 1" à 48 MHz, pour l'USB.
	GCLK -> GENCTRL [ GENERIC_CLOCK_GENERATOR_48M ].reg = GCLK_GENCTRL_SRC ( GCLK_GENCTRL_SRC_DFLL ) |
							      GCLK_GENCTRL_IDC |
							      //GCLK_GENCTRL_OE |
							      GCLK_GENCTRL_GENEN;
	while ( GCLK -> SYNCBUSY.reg & GCLK_SYNCBUSY ( GENERIC_CLOCK_GENERATOR_48M ) );

	// Configuration de XOSC0
	OSCCTRL -> XOSCCTRL [ 0 ].reg = OSCCTRL_XOSCCTRL_STARTUP ( 0x4 ) |
					OSCCTRL_XOSCCTRL_IMULT ( 5 ) |
					OSCCTRL_XOSCCTRL_IPTAT ( 3 ) |
					OSCCTRL_XOSCCTRL_ONDEMAND |
					OSCCTRL_XOSCCTRL_XTALEN |
					OSCCTRL_XOSCCTRL_ENABLE;
	while ( OSCCTRL -> STATUS.reg & OSCCTRL_STATUS_XOSCRDY0 );

	// Utilisation de l'oscillateur externe à 24MHz comme source pour "Generic Clock Generator 6"
	GCLK -> GENCTRL [ GENERIC_CLOCK_GENERATOR_24M ].reg = GCLK_GENCTRL_SRC ( GCLK_GENCTRL_SRC_XOSC0 ) |
							      GCLK_GENCTRL_IDC |
							      GCLK_GENCTRL_GENEN;
	while ( GCLK -> SYNCBUSY.reg & GCLK_SYNCBUSY ( GENERIC_CLOCK_GENERATOR_24M ) );

	// Configuration de DPLL0 pour utiliser l'oscillateur externe de 24MHz, réduit à 1MHz.
	OSCCTRL -> Dpll [ 0 ].DPLLRATIO.reg = OSCCTRL_DPLLRATIO_LDR ( 119 ) |							// Fdpll# = Freq_ref x ( LDR + 1 + ( LDRFRAC / 32 ) ) = 120MHz / 1MHz = 120 - 1 = 119
					      OSCCTRL_DPLLRATIO_LDRFRAC ( 0 );
	while ( OSCCTRL -> Dpll [ 0 ].DPLLSYNCBUSY.bit.DPLLRATIO );

	OSCCTRL -> Dpll [ 0 ].DPLLCTRLB.reg = OSCCTRL_DPLLCTRLB_DIV ( 11 ) |							// Freq_ref = F_XOSC / ( 2 x ( Div + 1 ) ) = 24MHz / ( 2 x ( 11 + 1 ) ) = 1MHz
					      OSCCTRL_DPLLCTRLB_LBYPASS |
					      OSCCTRL_DPLLCTRLB_REFCLK_XOSC0;

	OSCCTRL -> Dpll [ 0 ].DPLLCTRLA.reg = OSCCTRL_DPLLCTRLA_ENABLE;
	while ( OSCCTRL -> Dpll [ 0 ].DPLLSTATUS.bit.CLKRDY == 0 || OSCCTRL -> Dpll [ 0 ].DPLLSTATUS.bit.LOCK == 0 );

	// Utilisation du DPLL0 comme source pour "Generic Clock Generator 8"
	GCLK -> GENCTRL [ GENERIC_CLOCK_GENERATOR_120M ].reg = GCLK_GENCTRL_SRC ( GCLK_GENCTRL_SRC_DPLL0 ) |
							       GCLK_GENCTRL_IDC |
							       GCLK_GENCTRL_GENEN;
	while ( GCLK -> SYNCBUSY.reg & GCLK_SYNCBUSY ( GENERIC_CLOCK_GENERATOR_120M ) );

	// Utilisation de l'oscillateur externe comme source pour "Generic Clock Generator 7"
	GCLK -> GENCTRL [ GENERIC_CLOCK_GENERATOR_1M6 ].reg = GCLK_GENCTRL_SRC ( GCLK_GENCTRL_SRC_XOSC0 ) |
							      GCLK_GENCTRL_DIV ( 15 ) |
							      GCLK_GENCTRL_IDC |
							      GCLK_GENCTRL_GENEN;
	while ( GCLK -> SYNCBUSY.reg & GCLK_SYNCBUSY ( GENERIC_CLOCK_GENERATOR_1M6 ) );

	// Une fois toutes les horloges configurées, on peut définir CLKDIV.
	MCLK -> CPUDIV.reg = MCLK_CPUDIV_DIV_DIV1;
	//MCLK -> APBBMASK.reg |= MCLK_APBBMASK_USB;
}*/

void mdfyClocks () {
	OSC32KCTRL -> OSCULP32K.reg |= OSC32KCTRL_OSCULP32K_EN1K | OSC32KCTRL_OSCULP32K_EN32K;				// Configuration de l'oscillateur interne de 32,768kHz

	// Configuration de XOSC0
	OSCCTRL -> XOSCCTRL [ 0 ].reg = OSCCTRL_XOSCCTRL_STARTUP ( 0x4 ) |
					OSCCTRL_XOSCCTRL_IMULT ( 5 ) |
					OSCCTRL_XOSCCTRL_IPTAT ( 3 ) |
					OSCCTRL_XOSCCTRL_ONDEMAND |
					OSCCTRL_XOSCCTRL_XTALEN |
					OSCCTRL_XOSCCTRL_ENABLE;
	while ( OSCCTRL -> STATUS.reg & OSCCTRL_STATUS_XOSCRDY0 );

	// Utilisation de l'oscillateur externe à 24 MHz comme source pour "Generic Clock Generator 6" ( 24 MHz )
	GCLK -> GENCTRL [ GENERIC_CLOCK_GENERATOR_24M ].reg = GCLK_GENCTRL_SRC ( GCLK_GENCTRL_SRC_XOSC0 ) |
							      GCLK_GENCTRL_IDC |
							      GCLK_GENCTRL_GENEN;
	while ( GCLK -> SYNCBUSY.reg & GCLK_SYNCBUSY ( GENERIC_CLOCK_GENERATOR_24M ) );

	// Utilisation de l'oscillateur externe à 24 MHz comme source pour "Generic Clock Generator 7" ( 1,6 MHz )
	GCLK -> GENCTRL [ GENERIC_CLOCK_GENERATOR_1M6 ].reg = GCLK_GENCTRL_SRC ( GCLK_GENCTRL_SRC_XOSC0 ) |
							      GCLK_GENCTRL_DIV ( 15 ) |
							      GCLK_GENCTRL_IDC |
							      GCLK_GENCTRL_GENEN;
	while ( GCLK -> SYNCBUSY.reg & GCLK_SYNCBUSY ( GENERIC_CLOCK_GENERATOR_1M6 ) );
}

/*
__attribute__ (( __aligned__ ( 16 ))) static DmaDescriptor _descriptor [ 3 ];
__attribute__ (( __aligned__ ( 16 ))) static DmaDescriptor _writeBack [ 3 ];

void DMA_init () {
	static bool _initialized = false;

	if ( ! _initialized ) {
		MCLK -> AHBMASK.bit.DMAC;

		DMAC -> BASEADDR.reg = ( uint32_t ) _descriptor;
		DMAC -> WRBADDR.reg = ( uint32_t ) _writeBack;
		DMAC -> CTRL.reg = DMAC_CTRL_LVLEN0 |
				   DMAC_CTRL_LVLEN1 |
				   DMAC_CTRL_LVLEN2 |
				   DMAC_CTRL_LVLEN3;

		_initialized = true;
	}
}

void DMA_add_channel ( uint8_t _source_, DmaDescriptor *_descriptor_ ) {
	if ( DMAC -> CTRL.bit.DMAENABLE ) DMAC -> CTRL.bit.DMAENABLE = 0;
	while ( DMAC -> CTRL.bit.DMAENABLE );

	memcpy ( &_descriptor [ _dma_channel ], _descriptor_, sizeof ( DmaDescriptor ) );

	//DMAC ->
}

void DMA_enable ()							{ DMAC -> CTRL.bit.DMAENABLE = 1; }

uint8_t DMA_channels ()							{ return _dma_channel; }
*/
