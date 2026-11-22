#pragma once

#include "Arduino.h"
#include "../../configuration.h"
#include "../board/enums.h"
#include "../USB/HID_Usage.h"
#include "../USB/HID_Strings.h"

//BEGIN - Macros communes aux interfaces de type capteurs.
#define HID_FEATURE_COMMON_SENSORS_MACRO(unit_exponent)		HID_USAGEPAGE_SENSORS_PAGE, \
								HID_UNIT_TYPE8 ( UNIT_SYSTEM_NONE ), \
								HID_UNIT_EXPONENT ( SENSOR_EXPONENT_0 ), \
								HID_SENSOR_PROPERTY_SENSOR_CONNECTION_TYPE, \
								HID_LOGICAL_MIN8 ( SENSOR_CONNECTION_TYPE_PC_INTEGRATED ), \
								HID_LOGICAL_MAX8 ( SENSOR_CONNECTION_TYPE_PC_EXTERNAL ), \
								HID_REPORT_SIZE ( 8 ), \
								HID_REPORT_COUNT ( 1 ), \
								HID_COLLECTION ( COLLECTION_LOGICAL ), \
									HID_SENSOR_CONNECTION_TYPE_PC_INTEGRATED, \
									HID_SENSOR_CONNECTION_TYPE_PC_ATTACHED, \
									HID_SENSOR_CONNECTION_TYPE_PC_EXTERNAL, \
									HID_FEATURE ( DA_ABS ), \
								HID_END_COLLECTION, \
								HID_SENSOR_PROPERTY_POWER_STATE, \
								HID_LOGICAL_MIN8 ( SENSOR_POWER_STATE_UNDEFINED ), \
								HID_LOGICAL_MAX8 ( SENSOR_POWER_STATE_D4_POWER_OFF ), \
								HID_REPORT_SIZE ( 8 ), \
								HID_REPORT_COUNT ( 1 ), \
								HID_COLLECTION ( COLLECTION_LOGICAL ), \
									HID_SENSOR_POWER_STATE_UNDEFINED, \
									HID_SENSOR_POWER_STATE_D0_FULL_POWER, \
									HID_SENSOR_POWER_STATE_D1_LOW_POWER, \
									HID_SENSOR_POWER_STATE_D2_STANDBY_POWER_WITH_WAKEUP, \
									HID_SENSOR_POWER_STATE_D3_SLEEP_WITH_WAKEUP, \
									HID_SENSOR_POWER_STATE_D4_POWER_OFF, \
									HID_FEATURE ( DA_ABS ), \
								HID_END_COLLECTION, \
								HID_SENSOR_PROPERTY_REPORTING_STATE, \
								HID_LOGICAL_MIN8 ( SENSOR_REPORTING_STATE_REPORT_NO_EVENTS ), \
								HID_LOGICAL_MAX8 ( SENSOR_REPORTING_STATE_ANYTIME ), \
								HID_REPORT_SIZE ( 8 ), \
								HID_REPORT_COUNT ( 1 ), \
								HID_COLLECTION ( COLLECTION_LOGICAL ), \
									HID_SENSOR_REPORTING_STATE_REPORT_NO_EVENTS, \
									HID_SENSOR_REPORTING_STATE_REPORT_ALL_EVENTS, \
									HID_SENSOR_REPORTING_STATE_REPORT_THRESHOLD_EVENTS, \
									HID_SENSOR_REPORTING_STATE_WAKE_ON_NO_EVENTS, \
									HID_SENSOR_REPORTING_STATE_WAKE_ON_ALL_EVENTS, \
									HID_SENSOR_REPORTING_STATE_WAKE_ON_THRESHOLD_EVENTS, \
									HID_SENSOR_REPORTING_STATE_ANYTIME, \
									HID_FEATURE ( DA_ABS ), \
								HID_END_COLLECTION, \
								HID_SENSOR_PROPERTY_SENSOR_STATUS, \
								HID_LOGICAL_MIN8 ( SENSOR_STATE_UNDEFINED ), \
								HID_LOGICAL_MAX8 ( SENSOR_STATE_ERROR ), \
								HID_REPORT_SIZE ( 8 ), \
								HID_REPORT_COUNT ( 1 ), \
								HID_COLLECTION ( COLLECTION_LOGICAL ), \
									HID_SENSOR_STATE_UNDEFINED, \
									HID_SENSOR_STATE_READY, \
									HID_SENSOR_STATE_NOT_AVAILABLE, \
									HID_SENSOR_STATE_NO_DATA, \
									HID_SENSOR_STATE_INITIALIZING, \
									HID_SENSOR_STATE_ACCESS_DENIED, \
									HID_SENSOR_STATE_ERROR, \
									HID_FEATURE ( DA_ABS ), \
								HID_END_COLLECTION, \
								HID_SENSOR_PROPERTY_REPORT_INTERVAL, \
								HID_LOGICAL_MIN8 ( 0 ), \
								HID_LOGICAL_MAX32 ( 0x7FFFFFFF ), \
								HID_REPORT_SIZE ( 32 ), \
								HID_REPORT_COUNT ( 1 ), \
								HID_FEATURE ( DV_ABS ), \
								HID_UNIT_EXPONENT ( unit_exponent ), \
								HID_SENSOR_PROPERTY_CHANGE_SENSITIVITY_ABSOLUTE, \
								HID_LOGICAL_MIN8 ( 0 ), \
								HID_LOGICAL_MAX32 ( 0xFFFF ), \
								HID_REPORT_SIZE ( 16 ), \
								HID_REPORT_COUNT ( 1 ), \
								HID_FEATURE ( DV_ABS )

#define HID_INPUT_COMMON_SENSORS_MACRO()	HID_USAGEPAGE_SENSORS_PAGE, \
						HID_UNIT_TYPE8 ( UNIT_SYSTEM_NONE ), \
						HID_UNIT_EXPONENT ( SENSOR_EXPONENT_0 ), \
						HID_SENSOR_EVENT_SENSOR_EVENT, \
						HID_LOGICAL_MIN8 ( SENSOR_EVENT_UNKNOWN ), \
						HID_LOGICAL_MAX8 ( SENSOR_EVENT_COMPLEX_TRIGGER ), \
						HID_REPORT_SIZE ( 8 ), \
						HID_REPORT_COUNT ( 1 ), \
						HID_COLLECTION ( COLLECTION_LOGICAL ), \
							HID_SENSOR_EVENT_UNKNOWN, \
							HID_SENSOR_EVENT_STATE_CHANGED, \
							HID_SENSOR_EVENT_PROPERTY_CHANGED, \
							HID_SENSOR_EVENT_DATA_UPDATED, \
							HID_SENSOR_EVENT_POLL_RESPONSE, \
							HID_SENSOR_EVENT_CHANGE_SENSITIVITY, \
							HID_SENSOR_EVENT_RANGE_MAXIMUM_REACHED, \
							HID_SENSOR_EVENT_RANGE_MINIMUM_REACHED, \
							HID_SENSOR_EVENT_HIGH_THRESHOLD_CROSS_UPWARD, \
							HID_SENSOR_EVENT_HIGH_THRESHOLD_CROSS_DOWNWARD, \
							HID_SENSOR_EVENT_LOW_THRESHOLD_CROSS_UPWARD, \
							HID_SENSOR_EVENT_LOW_THRESHOLD_CROSS_DOWNWARD, \
							HID_SENSOR_EVENT_ZERO_THRESHOLD_CROSS_UPWARD, \
							HID_SENSOR_EVENT_ZERO_THRESHOLD_CROSS_DOWNWARD, \
							HID_SENSOR_EVENT_PERIOD_EXCEEDED, \
							HID_SENSOR_EVENT_FREQUENCY_EXCEEDED, \
							HID_SENSOR_EVENT_COMPLEX_TRIGGER, \
							HID_INPUT ( DA_ABS ), \
						HID_END_COLLECTION, \
						HID_SENSOR_EVENT_SENSOR_STATE, \
						HID_LOGICAL_MIN8 ( SENSOR_STATE_UNDEFINED ), \
						HID_LOGICAL_MAX8 ( SENSOR_STATE_ERROR ), \
						HID_REPORT_SIZE ( 8 ), \
						HID_REPORT_COUNT ( 1 ), \
						HID_COLLECTION ( COLLECTION_LOGICAL ), \
							HID_SENSOR_STATE_UNDEFINED, \
							HID_SENSOR_STATE_READY, \
							HID_SENSOR_STATE_NOT_AVAILABLE, \
							HID_SENSOR_STATE_NO_DATA, \
							HID_SENSOR_STATE_INITIALIZING, \
							HID_SENSOR_STATE_ACCESS_DENIED, \
							HID_SENSOR_STATE_ERROR, \
							HID_INPUT ( DA_ABS ), \
						HID_END_COLLECTION
//END

static const uint8_t _Device1_HIDReportDescriptor [] PROGMEM = {
	//#define HID_EXTENDED_PAGE_ACTIVE 0x20
	HID_USAGEPAGE_SENSORS_PAGE,								// USAGE PAGE ( Sensors Page )
	HID_SENSOR_SENSOR,
	HID_COLLECTION ( COLLECTION_APPLICATION ),
		//BEGIN - Les capteurs de températures...
		HID_USAGEPAGE_SENSORS_PAGE,							//	USAGE PAGE ( Sensors Page )
		HID_SENSOR_ENVIRONMENTAL_TEMPERATURE,						//	USAGE ( Environmentals: Temperature )
		HID_COLLECTION ( COLLECTION_PHYSICAL ),
			HID_REPORT_ID ( Sensors_TemperatureReport_ID ),
			//BEGIN - Les FEATURES spécifiques aux capteurs de température
			HID_FEATURE_COMMON_SENSORS_MACRO ( SENSOR_UNIT_CENTI ),
			//END



			//BEGIN - INPUT Report. Les températures envoyées vers le PC.
			HID_INPUT_COMMON_SENSORS_MACRO (),

			HID_SENSOR_DATAFIELD_TEMPERATURE,					//		Température SAM-E51
			HID_SENSOR_DATAFIELD_TEMPERATURE,					//		Température boîtier ( Si sonde branchée )
			HID_LOGICAL_MIN16 ( 0x8000 ),						//		Valeur minimum: -327,68°C
			HID_LOGICAL_MAX16 ( 0x7FFF ),						//		Valeur maximum:  327,67°C
			HID_REPORT_SIZE ( 16 ),							//		Poids des valeurs, 16 bits.
			HID_REPORT_COUNT ( 2 ),							//		Nombre de valeurs reçues.
			HID_UNIT_EXPONENT ( SENSOR_UNIT_CENTI ),				//
			HID_INPUT ( DV_ABS ),
			//END
		HID_END_COLLECTION,
		//END



		//BEGIN - Les capteurs de tension...
		HID_USAGEPAGE_SENSORS_PAGE,
		HID_SENSOR_ELECTRICAL_VOLTAGE,
		HID_COLLECTION ( COLLECTION_PHYSICAL ),
			HID_REPORT_ID ( Sensors_MonitorReport_ID ),
			//BEGIN - Les FEATURES spécifiques aux capteurs de tension
			HID_FEATURE_COMMON_SENSORS_MACRO ( SENSOR_UNIT_MILLI ),
			//END



			//BEGIN - INPUT Report. Les tensions envoyées vers le PC. struct voltage_t dans "../sensors/monitor.h"
			HID_INPUT_COMMON_SENSORS_MACRO (),

			HID_REPORT_SIZE ( 8 ),							//		Poids des valeurs, 8 bits.
			HID_REPORT_COUNT ( 2 ),							//		Nombre de valeurs envoyées.
			HID_INPUT ( CV_ABS ),							//		L'état de mise à jour des tensions. 'State'	Non utilisé par le HID, considéré comme du Padding !

			HID_USAGEPAGE_SENSORS_PAGE,
			HID_SENSOR_DATAFIELD_VOLTAGE,						//		Tension +1,2V de cœur
			HID_SENSOR_DATAFIELD_VOLTAGE,						//		Tension +3V de la pile de backup
			HID_SENSOR_DATAFIELD_VOLTAGE,						//		Tension +3,3V sur les broches I/O
			HID_SENSOR_DATAFIELD_VOLTAGE,						//		Tension +3,3V de référence
			HID_SENSOR_DATAFIELD_VOLTAGE,						//		Tension +5V d'alimentation de la carte
			HID_SENSOR_DATAFIELD_VOLTAGE,						//		Tension +3,3V d'alimentation
			HID_SENSOR_DATAFIELD_VOLTAGE,						//		Tension de déchet sur la masse
			HID_LOGICAL_MIN8 ( 0 ),							//		Valeur minimum: 0V
			HID_LOGICAL_MAX16 ( 6000 ),						//		Valeur maximum: 6V
			HID_REPORT_SIZE ( 16 ),							//		Poids des valeurs, 16 bits.
			HID_REPORT_COUNT ( 7 ), 						//		Nombre de valeurs reçues.
			HID_UNIT_EXPONENT ( SENSOR_UNIT_MILLI ),				//		Unité des valeurs, milliVolts.
			HID_INPUT ( DV_ABS ),
			//END
		HID_END_COLLECTION,
		//END
	HID_END_COLLECTION
	//#undef HID_EXTENDED_PAGE_ACTIVE
};

static const uint8_t _Device2_HIDReportDescriptor [] PROGMEM = {
	#define UNIT_LITTER_PER_HOUR		UNIT_NIBBLES(UNIT_SYSTEM_SI_LINEAR, 3, UNIT_NULL, UNIT_EXPONENT_MINUS_1, UNIT_NULL, UNIT_NULL, UNIT_NULL)
	HID_USAGEPAGE_SENSORS_PAGE,
	HID_SENSOR_SENSOR,
	HID_COLLECTION (COLLECTION_APPLICATION ),
		//BEGIN - Le capteur de température de liquide ( AquaComputer High-Flow 2 )
		HID_USAGEPAGE_SENSORS_PAGE,							//	USAGE PAGE ( Sensors Page )
		HID_SENSOR_ENVIRONMENTAL_TEMPERATURE,
		HID_COLLECTION ( COLLECTION_PHYSICAL ),
			HID_REPORT_ID ( HighFlow2_TemperatureReport_ID ),
			//BEGIN - Les FEATURES spécifiques aux capteurs de température
			HID_FEATURE_COMMON_SENSORS_MACRO ( SENSOR_UNIT_CENTI ),
			//END



			//BEGIN - INPUT Report. Les températures envoyées vers le PC.
			HID_INPUT_COMMON_SENSORS_MACRO (),

			HID_SENSOR_DATAFIELD_TEMPERATURE,				//	Température liquide
			HID_LOGICAL_MIN16 ( 0x8000 ),					//	Valeur minimum: -327,68°C
			HID_LOGICAL_MAX16 ( 0x7FFF ),					//	Valeur maximum:  327,67°C
			HID_REPORT_SIZE ( 16 ),						//	Poids des valeurs, 16 bits.
			HID_REPORT_COUNT ( 1 ),						//	Nombre de valeurs reçues.
			HID_UNIT_EXPONENT ( SENSOR_UNIT_CENTI ),			//
			HID_INPUT ( DV_ABS ),
			//END
		HID_END_COLLECTION,
		//END



		//BEGIN - Le débimètre ( AquaComputer High-Flow 2 )
		HID_USAGEPAGE_SENSORS_PAGE,
		HID_SENSOR_OTHER_GENERIC,						//	USAGE ( Other: Generic )
		HID_COLLECTION ( COLLECTION_PHYSICAL ),
			HID_REPORT_ID ( HighFlow2_FlowReport_ID ),
			//BEGIN - Les FEATURES spécifiques pour le capteur de débit
			HID_FEATURE_COMMON_SENSORS_MACRO ( SENSOR_UNIT ),

			HID_SENSOR_PROPERTY_FRIENDLY_NAME,
			HID_LOGICAL_MIN8 ( 0 ),
			HID_LOGICAL_MAX16 ( 0xFF ),
			HID_REPORT_SIZE ( 8 ),
			HID_REPORT_COUNT ( 32 ),
			HID_FEATURE ( DV_ABS ),
			//END



			//BEGIN - INPUT Report. Le débit envoyé vers le PC.
			HID_INPUT_COMMON_SENSORS_MACRO (),

			HID_SENSOR_DATAFIELD_CUSTOM_VALUE_1,
			HID_LOGICAL_MIN16 ( 0xFFFF ),					//	Valeur minimum:     -1 L/h.
			HID_LOGICAL_MAX16 ( 0x7FFF ),					//	Valeur maximum:  32768 L/h.
			HID_PHYSICAL_MIN16 ( -278 ),					//	Valeur physique minimum:   -0,278 cm³/s.
			HID_PHYSICAL_MAX32 ( 9101944 ),					//	Valeur physique maximum: 9101,944 cm³/s.
			HID_UNIT_TYPE32 ( UNIT_LITTER_PER_HOUR ),			//	Unité SI: cm³/s.
			HID_UNIT_EXPONENT ( UNIT_EXPONENT_MINUS_3 ),			//	Exposant 10^-3 appliqué aux valeurs physiques.
			HID_REPORT_SIZE ( 16 ),						//	Poids des valeurs, 16 bits.
			HID_REPORT_COUNT ( 1 ),						//	Nombre de valeurs à envoyer.
			HID_INPUT ( DV_ABS ),
			//END
		HID_END_COLLECTION,
		//END
	HID_END_COLLECTION
};

static const uint8_t _Device3_HIDReportDescriptor [] PROGMEM = {
	HID_USAGEPAGE_VENDOR_SPECIFIC,
	HID_VENDOR_SPECIFIC_UNDEFINED,

	HID_COLLECTION ( COLLECTION_APPLICATION ),
		//BEGIN - FEATURE Report.
		//BEGIN - Numéro de Firmware.							RO
		HID_COLLECTION ( COLLECTION_LOGICAL ),
			HID_REPORT_ID ( Firmware_ID ),
			HID_USAGEPAGE_GENERIC_DEVICE_CONTROLS_PAGE,
			HID_GENERICDEVICE_SOFTWARE_VERSION,
			HID_LOGICAL_MIN8 ( 0x00 ),
			HID_LOGICAL_MAX16 ( 0xFF ),

			HID_GENERICDEVICE_MINOR,
			HID_REPORT_SIZE ( 8 ),
			HID_REPORT_COUNT ( 1 ),
			HID_FEATURE ( CV_ABS ),

			HID_GENERICDEVICE_MAJOR,
			HID_REPORT_SIZE ( 8 ),
			HID_REPORT_COUNT ( 1 ),
			HID_FEATURE ( CV_ABS ),
		HID_END_COLLECTION,
		//END

		//BEGIN - Enregistrement du Timestamp.						WO
		HID_COLLECTION ( COLLECTION_LOGICAL ),
			HID_REPORT_ID ( TimeStamp_ID ),
			HID_USAGEPAGE_SENSORS_PAGE,
			HID_SENSOR_TIME,
			HID_LOGICAL_MIN8 ( 0x00 ),
			HID_LOGICAL_MAX32 ( 0x7FFFFFFF ),
			HID_REPORT_SIZE ( 32 ),
			HID_REPORT_COUNT ( 2 ),

			HID_SENSOR_DATAFIELD_TIMESTAMP,
			HID_SENSOR_DATAFIELD_TIMESTAMP,
			HID_FEATURE ( DV_ABS ),
		HID_END_COLLECTION,
		//END

		//BEGIN - Paramétrage de l'horloge.						RW
		//END

		//BEGIN - Paramétrage des offsets de jours.					RW
		//END

		//BEGIN - Paramétrage de l'affichage.						RW
		//END

		//BEGIN - Paramétrage du dialogue.						RW
		//END

		//BEGIN - Reset de l'affichage ou du système.					WO
		HID_REPORT_ID ( Reset_ID ),
		HID_USAGEPAGE_VENDOR_SPECIFIC,
		HID_VENDOR_SPECIFIC_RESET,
		HID_LOGICAL_MIN8 ( 0x00 ),
		HID_LOGICAL_MAX16 ( 0xFF ),
		HID_REPORT_SIZE ( 8 ),
		HID_REPORT_COUNT ( 1 ),
		HID_FEATURE ( CV_ABS ),
		//END
		//END



		//BEGIN - OUTPUT Report.
		HID_COLLECTION ( COLLECTION_LOGICAL ),
			HID_REPORT_ID ( Screen_OutputReport_ID ),
			HID_USAGEPAGE_SENSORS_PAGE,
			HID_SENSOR_ENVIRONMENTAL_TEMPERATURE,
			//BEGIN - Les températures envoyées depuis le PC. struct external_temperatures_t dans "../board/enums.h"
			HID_COLLECTION ( COLLECTION_PHYSICAL ),
				HID_USAGEPAGE_SENSORS_PAGE,						//		USAGE PAGE ( Sensors Page )

				HID_SENSOR_DATAFIELD_TEMPERATURE,					//		Température du processeur
				HID_SENSOR_DATAFIELD_TEMPERATURE,					//		Température de la carte graphique n°1
				HID_SENSOR_DATAFIELD_TEMPERATURE,					//		Température de la carte graphique n°2
				HID_LOGICAL_MIN8 ( 0x80 ),						//		Valeur minimum: -128°C
				HID_LOGICAL_MAX8 ( 0x7F ),						//		Valeur maximum:  127°C
				HID_REPORT_SIZE ( 8 ),							//		Poids des valeurs, 8 bits.
				HID_REPORT_COUNT ( 3 ),							//		Nombre de valeurs reçues.
				HID_OUTPUT ( DV_ABS ),

				HID_SENSOR_DATAFIELD_TEMPERATURE,					//		Température moyenne zone arrière
				HID_SENSOR_DATAFIELD_TEMPERATURE,					//		Température moyenne zone supérieure
				HID_SENSOR_DATAFIELD_TEMPERATURE,					//		Température moyenne zone frontale
				HID_SENSOR_DATAFIELD_TEMPERATURE,					//		Température moyenne zone HDD
				HID_LOGICAL_MIN16 ( 0x8000 ),						//		Valeur minimum: -327,68°C
				HID_LOGICAL_MAX16 ( 0x7FFF ),						//		Valeur maximum:  327,67°C
				HID_REPORT_SIZE ( 16 ),							//		Poids des valeurs, 16 bits.
				HID_REPORT_COUNT ( 4 ),							//		Nombre de valeurs reçues.
				HID_OUTPUT ( DV_ABS ),

				HID_SENSOR_UNDEFINED,
				HID_REPORT_SIZE ( 8 ),							//		Poids des valeurs, 8 bits.
				HID_REPORT_COUNT ( 1 ),							//		Nombre de valeurs reçues.
				HID_OUTPUT ( HID_CONSTANT ),						//		Padding
			HID_END_COLLECTION,
			//END

			HID_USAGEPAGE_GENERIC_DESKTOP_PAGE,
			HID_GENERIC_TYPE_COLLECTION_WATERCOOLING,
			//BEGIN - Les vitesses envoyées depuis le PC. struct external_speeds_t dans "../board/enums.h"
			HID_COLLECTION ( COLLECTION_PHYSICAL ),
				HID_USAGEPAGE_GENERIC_DESKTOP_PAGE,

				HID_GENERIC_RPM,							//		Vitesse moyenne des pompes
				HID_GENERIC_RPM,							//		Vitesse du ventilateur arrière
				HID_GENERIC_RPM,							//		Vitesse moyenne des ventilateurs supérieurs
				HID_GENERIC_RPM,							//		Vitesse du ventilateur frontal
				HID_GENERIC_RPM,							//		Vitesse moyenne des ventilateurs de la zone HDD
				HID_LOGICAL_MIN16 ( 0xFFFF ),
				HID_LOGICAL_MAX16 ( 0x7FFF ),
				HID_REPORT_SIZE ( 16 ),
				HID_REPORT_COUNT ( 5 ),
				HID_OUTPUT ( DV_ABS ),
			HID_END_COLLECTION,
			//END
		HID_END_COLLECTION,
		//END



		//BEGIN - INPUT Report
		HID_COLLECTION (COLLECTION_LOGICAL ),
			HID_REPORT_ID ( Screen_FeatureReport_ID ),

			//BEGIN - Information sur la modification d'une Feature.
			HID_USAGEPAGE_GENERIC_DESKTOP_PAGE,
			HID_GENERIC_FEATURE_NOTIFICATION,
			HID_LOGICAL_MIN8 ( 0x00 ),
			HID_LOGICAL_MAX16 ( 0xFF ),
			HID_REPORT_SIZE ( 8 ),
			HID_REPORT_COUNT ( 1 ),
			HID_INPUT ( DV_ABS ),
			//END

			//BEGIN - Padding
			HID_GENERIC_UNDEFINED,
			HID_REPORT_SIZE ( 8 ),
			HID_REPORT_COUNT ( 1 ),
			HID_INPUT ( HID_CONSTANT ),
			//END
		HID_END_COLLECTION,
		//END
	HID_END_COLLECTION
};

