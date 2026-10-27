#include "Arduino.h"
#include "./watchdog.h"

/** @brief	Paramétrage du watchdog.
 *  @details	Le watchdog utilise la sortie 1,024kHz de l'oscillateur interne 32,768kHz ULP.
 */
void CLASS_WATCHDOG_::begin () {
	WDT -> CONFIG.reg = WDT_CONFIG_PER ( 0xB );											// Période de 16384 cycles d'horloge, soit 16 secondes pour une horloge de 1,024kHz.
	WDT -> CTRLA.reg = //WDT_CTRLA_ALWAYSON |
			   WDT_CTRLA_ENABLE;
}

/** @brief	Relance le compteur du watchdog...
 *  @details
 */
void CLASS_WATCHDOG_::Reset () {
	if ( ! WDT -> SYNCBUSY.reg ) WDT -> CLEAR.reg = 0xA5;										// Écriture de 0xA5. Voir Datasheet !
}

CLASS_WATCHDOG_ watchDog_;

