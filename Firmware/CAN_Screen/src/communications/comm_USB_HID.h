#pragma once

#include "../board/enums.h"
#include "../board/macros.h"
#include "../screen/screen.h"
#include "../sensors/flow.h"
#include "../sensors/monitor.h"
#include "../sensors/temperature.h"
#include "../USB/hid_device.h"
#include "../USB/HID_Global.h"
#include "../USB/HID_Usage.h"

#define MAX_FEATURE_REPORT		4

#define SENSORS_DEVICE_NUMBER		(SENSORS_HID_INTERFACE_NUMBER - CDC_INTERFACE_COUNT)
#define HIGHFLOW2_DEVICE_NUMBER		(HIGHFLOW2_HID_INTERFACE_NUMBER - CDC_INTERFACE_COUNT)
#define SCREEN_DEVICE_NUMBER		(SCREEN_HID_INTERFACE_NUMBER - CDC_INTERFACE_COUNT)

typedef struct {
	uint8_t featuresID [ MAX_FEATURE_REPORT ];
	uint8_t queue;
	bool locked;

	bool can_Read ()				{ return ( ( this -> queue > 0 ) && ( this -> locked == false ) ); }
	uint8_t get () {
		uint8_t _ID = 0;
		if ( this -> can_Read () ) {
			_ID = this -> featuresID [ 0 ];								// Enregistrement de l'ID de la feature.

			memmove ( this -> featuresID, this -> featuresID + 1, MAX_FEATURE_REPORT - 1 );		// Déplacement du tampon.
			this -> featuresID [ MAX_FEATURE_REPORT - 1 ] = 0;
			this -> queue --;
			this -> locked = true;									// On bloque le tampon pour ne pas spammer le port USB !
		}
		return _ID;
	}
	void add ( const uint8_t _featureID_ ) {
		if ( this -> queue < MAX_FEATURE_REPORT ) {							// Pour être certain de ne pas déborder !
			for ( uint8_t _pos = 0; _pos < MAX_FEATURE_REPORT; _pos ++ ) {				// Vérification de tout le tampon
				if ( this -> featuresID [ _pos ] == _featureID_ ) break;			// La 'featuresID' est déjà présente ? Alors on quitte !
				if ( this -> featuresID [ _pos ] == 0 ) {					// Première position de vide.
					this -> featuresID [ _pos ] = _featureID_;				// Ajout de l'ID.
					this -> queue = _pos + 1;						// Et incrémentation du compteur.
					break;									// Puis on quitte la boucle de vérification !
				}
			}
		}
		else this -> queue = MAX_FEATURE_REPORT;
	}
} featuresUpdated_t;

typedef struct {
	uint8_t Sensors_TemperatureDevice: 1;
	uint8_t Sensors_MonitorDevice: 1;
	__I uint8_t : 1;
	uint8_t HighFlow2_TemperatureDevice: 1;
	uint8_t HighFlow2_FlowDevice: 1;
	__I uint8_t : 1;
	uint8_t Screen_Device: 1;
	__I uint8_t : 1;
} packed_t;

_Pragma("pack(1)")
	typedef struct {
		uint8_t Connection;								// Taille:  1 Byte
		uint8_t PowerState;								// Taille:  1 Byte
		uint8_t State;									// Taille:  1 Byte
		uint8_t Status;									// Taille:  1 Byte
		uint32_t Interval;								// Taille:  4 Bytes
		uint16_t Sensivity;								// Taille:  2 Bytes
	} features_t;										// Total:  10 Bytes !

	typedef struct {
		uint8_t Firmware_Minor;								// Taille:  1 Byte
		uint8_t Firmware_Major;								// Taille:  1 Byte
	} firmware_t;										// Total:   2 Bytes !

	typedef struct {
		uint8_t Mode_24h:        1;							// ../board/rtc.h -> CLASS_RTC_
		uint8_t Mode_AutoGMT:    1;							// ../board/rtc.h -> CLASS_RTC_
		__I uint8_t :            2;							// Padding
		uint8_t Clock:           2;							// struct clock_view_t dans "../screen/screen.h"
		__I uint8_t :            2;							// Padding
	} clock_config_t;									// Total:   1 Byte !

	/* Device 1 - Sensor device !								*/
	#if ENABLED(SENSORS_DEVICE)
		typedef struct {
			//uint8_t Status;
			//uint8_t Event;
			union {
				struct {
					int16_t Chip;
					int16_t Box;
				};
				int16_t Temperature [ 2 ];
			};
		} hid_temperature_t;
		typedef struct {
			uint8_t Status;
			uint8_t Event;
			voltage_t voltages_;
		} hid_monitor_t;
		/*typedef struct {
			hid_temperature_t hid_temps_;
			hid_monitor_t hid_monitor_;
		} HID_Device1_Input_Data_t;*/
		typedef struct {
			union {
				struct {
					int16_t Chip;						// Taille: *2 Bytes
					int16_t Box;						// Taille: *2 Bytes
				};								// Taille: *4 Bytes
				int16_t Temperatures [ 2 ];					// Taille: *4 Bytes
			};									// Taille:  4 Bytes
			voltage_t voltages_;							// Taille: 16 Bytes
		} HID_Device1_Input_Data_t;							// Total:  20 Bytes !

		typedef struct {
			features_t temperatures_features_;					// Taille: 10 Bytes
			features_t voltages_features_;						// Taille: 10 Bytes
		} HID_Device1_Features_Data_t;							// Total:  20 Bytes !
	#endif

	/* Device 2 - AquaComputer High-Flow 2 !						*/
	#if ENABLED(HIGHFLOW2_DEVICE)
		typedef struct {
			int16_t Temperature;							// Taille:  2 Bytes
			high_flow_t high_flow_;							// Taille:  2 Bytes
		} HID_Device2_Input_Data_t;							// Total:   4 Bytes !

		typedef struct {
			features_t temperatures_features_;					// Taille: 10 Bytes
			features_t flow_features_;						// Taille: 10 Bytes
		} HID_Device2_Features_Data_t;							// Total:  20 Bytes !
	#endif

	/* Device 3 - Screen device !								*/
	#if ENABLED(SCREEN_DEVICE)
		typedef struct {
			uint8_t Feature_Update;							// Taille:  1 Bytes
			__I uint8_t Padding;							//				Padding !
		} HID_Device3_Input_Data_t;							// Total:   2 Bytes !

		typedef struct {
			external_temperatures_t temperatures_;					// Taille: 12 Bytes
			external_speeds_t vitesses_;						// Taille: 10 Bytes
		} HID_Device3_Output_Data_t;							// Total:  22 Bytes !

		typedef struct {
			firmware_t firmware_;							// Taille:  2 Bytes
			uint64_t Timestamp;							// Taille:  8 Bytes
			clock_config_t clock_config_;						// Taille:  1 Byte
		} HID_Device3_Features_Data_t;							// Total:
	#endif
_Pragma("pack()")

class CLASS_HID_ {
private:
	#if ENABLED(SENSORS_DEVICE)
		HID_Device1_Input_Data_t sensors_interrupt_IN_;
		HID_Device1_Features_Data_t sensors_features_;
		uint32_t _sensors_temperature_timing;
		uint32_t _sensors_monitor_timing;
	#endif

	#if ENABLED(HIGHFLOW2_DEVICE)
		HID_Device2_Input_Data_t highflow2_interrupt_IN_;
		HID_Device2_Features_Data_t highflow2_features_;
		uint32_t _highflow2_temperature_timing;
		uint32_t _highflow2_flow_timing;
	#endif

	#if ENABLED(SCREEN_DEVICE)
		HID_Device3_Input_Data_t screen_interrupt_IN_;
		HID_Device3_Output_Data_t screen_interrupt_OUT_;
		HID_Device3_Features_Data_t screen_features_;
		uint32_t _screen_timing;
	#endif

	packed_t _report_Packed;

	void loop_InputReport ();
	void loop_OutputReport ();
	void loop_SetReport ();

public:
	CLASS_HID_ ();
	void begin ();
	void loop ();
};

extern CLASS_HID_ hid_;
