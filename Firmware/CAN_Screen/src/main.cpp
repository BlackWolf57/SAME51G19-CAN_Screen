#include "Arduino.h"
#include "./board/clocks.h"
#include "./board/rtc.h"
#include "./communications/comm_USB_HID.h"
#include "./screen/screen.h"
#include "./sensors/flow.h"
#include "./sensors/monitor.h"
#include "./sensors/temperature.h"
#include "../configuration.h"
#if ENABLED(USE_WATCHDOG)
	#include "./board/watchdog.h"
#endif

extern USBDeviceClass USBDevice;
extern "C" void __libc_init_array ( void );
extern uint32_t __etext;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;

void Reset_Handler ( void ) {
	uint32_t *pSrc, *pDest;

	pSrc = &__etext;
	pDest = &__data_start__;
	if ( ( &__data_start__ != &__data_end__ ) && ( pSrc != pDest ) ) {
		for ( ; pDest < &__data_end__; pDest++, pSrc++ ) {
			*pDest = *pSrc;
		}
	}

	if ( ( &__data_start__ != &__data_end__ ) && ( pSrc != pDest ) ) {
		for ( pDest = &__bss_start__; pDest < &__bss_end__; pDest++ ) {
			*pDest = 0;
		}
	}

	#if defined(__FPU_USED)
		SCB -> CPACR |= ( 0xFu << 20 );
		__DSB ();
		__ISB ();
	#endif

	//if ( ! bitRead ( RTC -> MODE2.GP [ 0 ].reg, 0 ) ) {
		SystemInit ();
		mdfyClocks ();
	//}

	main ();

	while ( 1 );
}

void initBattery () {
	if ( ( ! RSTC -> RCAUSE.bit.BACKUP ) && ( ! RSTC -> BKUPEXIT.bit.BBPS ) ) {
		SUPC -> BOD33.bit.ENABLE = 0;
		while ( ! SUPC -> STATUS.reg & SUPC_STATUS_B33SRDY );

		SUPC -> BOD33.reg |= SUPC_BOD33_ACTION ( SUPC_BOD33_ACTION_BKUP ) |
				     //SUPC_BOD33_VBATLEVEL ( 28 ) |
				     //SUPC_BOD33_RUNHIB |
				     SUPC_BOD33_RUNBKUP;
	}

	//SUPC -> VREG.reg |= SUPC_VREG_RUNBKUP;
	SUPC -> BBPS.reg = SUPC_BBPS_WAKEEN;

	SUPC -> BOD33.reg |= SUPC_BOD33_ENABLE;
	while ( ! SUPC -> STATUS.reg & SUPC_STATUS_B33SRDY );
}

void print_RCause () {
	uint8_t _rCause = RSTC -> RCAUSE.reg;
	uint8_t _bkupExit = RSTC -> BKUPEXIT.reg;

	if ( bitRead ( _rCause, 0 ) ) SerialUSB.println ( "Power On Reset !" );
	if ( bitRead ( _rCause, 1 ) ) SerialUSB.println ( "Brown Out 12 Detector Reset !" );
	if ( bitRead ( _rCause, 2 ) ) SerialUSB.println ( "Brown Out 33 Detector Reset !" );
	if ( bitRead ( _rCause, 3 ) ) SerialUSB.println ( "NVM Reset !" );
	if ( bitRead ( _rCause, 4 ) ) SerialUSB.println ( "External Reset !" );
	if ( bitRead ( _rCause, 5 ) ) SerialUSB.println ( "WatchDog Reset !" );
	if ( bitRead ( _rCause, 6 ) ) SerialUSB.println ( "System Reset Request !" );
	if ( bitRead ( _rCause, 7 ) ) {
		if ( bitRead ( _bkupExit, 1 ) ) SerialUSB.println ( "Real Time Counter Interrupt." );
		if ( bitRead ( _bkupExit, 2 ) ) SerialUSB.println ( "Battery Backup Power Switch." );
		if ( bitRead ( _bkupExit, 7 ) ) SerialUSB.println ( "Hibernate." );
	}
}

int main ( void ) {
	#if ENABLED(USE_WATCHDOG)
		watchDog_.begin ();
	#endif

	init ();
	__libc_init_array ();
	delay ( 1 );

	#if defined(USBCON)
		USBDevice.init ();
		USBDevice.attach ();
	#endif

	SerialUSB.begin ( 115200 );
	SerialUSB.setTimeout ( 1000 );
	/*do {
		if ( millis () > 2500 ) break;
	} while ( !SerialUSB );*/
	hid_.begin ();

	initBattery ();
	screen_.begin ();
	//screen_.loading ();

	screen_.loading ( rtc_.begin () );
	screen_.loading ( monitor_.begin () );
	screen_.loading ( temperatures_.begin () );
	screen_.loading ( debimetre_.begin () );

	//if ( ( ! RSTC -> RCAUSE.bit.BACKUP ) && ( ! RSTC -> BKUPEXIT.bit.BBPS ) ) rtc_.set_TimeStamp ( 1665395664 );							// 10/10/2022	09 ( + 1 ):54:24
	if ( ( ! RSTC -> RCAUSE.bit.BACKUP ) && ( ! RSTC -> BKUPEXIT.bit.BBPS ) ) rtc_.set_TimeStamp ( 1782893203 );							// 01/07/2026	10:06:41
	static uint32_t _now = 0;
	delay ( 200 );
	print_RCause ();
	SerialUSB.println ( "SAME51G19 - Initialisation OK !" );
	SerialUSB.println ( "" );
	SerialUSB.println ( "" );

	for ( ; ; ) {
		#if ENABLED(USE_WATCHDOG)
			watchDog_.Reset ();
		#endif

		rtc_.loop ();									// Pour le suivi de la synchronisation de l'heure.
		monitor_.loop ();								// En priorité, pour avoir la tension de référence et la tension de déchêt !
		temperatures_.loop ();								// Les mesures de température !
		screen_.loop ();
		hid_.loop ();

		if ( ( ( time_.seconds < 5 ) && ( _now == 0 ) ) || ( time_.seconds > ( _now + 5 ) ) ) {
			SerialUSB.println ( "Températures:" );
			SerialUSB.print ( " - µC = " );
			SerialUSB.print ( temperatures_.Chip (), 2 );
			SerialUSB.println ( " °C" );
			SerialUSB.print ( " - Boitier = " );
			SerialUSB.print ( temperatures_.Box (), 2 );
			SerialUSB.println ( "°C" );
			SerialUSB.print ( " - Liquide = " );
			SerialUSB.print ( temperatures_.Water (), 2 );
			SerialUSB.println ( "°C" );

			SerialUSB.println ( "" );
			SerialUSB.println ( "Tensions:" );
			SerialUSB.print ( " - Tension de référence = " );
			SerialUSB.print ( monitor_.VRef (), DEC );
			SerialUSB.println ( " mV" );
			SerialUSB.print ( " - +5V = " );
			SerialUSB.print ( monitor_.VPower (), DEC );
			SerialUSB.println ( " mV" );
			SerialUSB.print ( " - +3,3V = " );
			SerialUSB.print ( monitor_.VLogic (), DEC );
			SerialUSB.println ( " mV" );
			SerialUSB.print ( " - Tension d'entré / sortie' = " );
			SerialUSB.print ( monitor_.IOVCC (), DEC );
			SerialUSB.println ( " mV" );
			SerialUSB.print ( " - Tension de coeur = " );
			SerialUSB.print ( monitor_.VCORE (), DEC );
			SerialUSB.println ( " mV" );
			SerialUSB.print ( " - Tension de la pile = " );
			SerialUSB.print ( monitor_.VBatt (), DEC );
			SerialUSB.println ( " mV" );
			SerialUSB.print ( " - Tension de déchet sur la masse = " );
			SerialUSB.print ( monitor_.VSSA (), DEC );
			SerialUSB.println ( " mV" );
			SerialUSB.print ( " - Valeur ADC de déchet sur la masse = " );
			SerialUSB.println ( monitor_.VSSA_ADC (), DEC );

			SerialUSB.println ( "" );
			SerialUSB.println ( "Débit / Vitesse:" );
			SerialUSB.print ( " - Débit du liquide = " );
			SerialUSB.print ( debimetre_.Flow (), DEC );
			SerialUSB.println ( " l/h" );
			SerialUSB.print ( " - Vitesse de rotation du débimètre = " );
			SerialUSB.print ( debimetre_.Speed (), DEC );
			SerialUSB.println ( " trs/min" );

			SerialUSB.println ( "" );
			SerialUSB.println ( "" );
			SerialUSB.print ( "RTC: " );
			SerialUSB.print ( rtc_.get_Hour (), DEC );
			SerialUSB.print ( ":" );
			SerialUSB.print ( rtc_.get_Minute (), DEC );
			SerialUSB.print ( ":" );
			SerialUSB.println ( rtc_.get_Second (), DEC );
			SerialUSB.print ( "SUPC -> BOD33 = 0x" );
			SerialUSB.print ( SUPC -> BOD33.reg, HEX );
			SerialUSB.print ( "\t\tSUPC -> BBPS = 0x" );
			SerialUSB.print ( SUPC -> BBPS.reg, HEX );
			SerialUSB.print ( "\t\tRTC -> MODE2.GP [ 0 ] = 0x" );
			SerialUSB.println ( RTC -> MODE2.GP [ 0 ].reg, HEX );
			SerialUSB.println ( "" );
			SerialUSB.println ( "" );

			_now = time_.seconds;
		}
	}

	return 0;
}

