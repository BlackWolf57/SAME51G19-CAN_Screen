/**
 * Petit texte de présentation...
 */


#pragma once

#include "./src/board/macros.h"

#define FIRMWARE_NUMBER					0x0103

#define DEBUG						// Affichage d'informations supplémentaires dans la console
//#define USE_WATCHDOG					// Utilisation du WatchDog pour éviter un blocage logiciel
//#define NO_EEPROM					// Désactive la mémoire pour certains tests.

//BEGIN - Paramétrage des différents ID !
#define HOST_ID				0xB0
#define DEVICE_ID			0xE5
//END
