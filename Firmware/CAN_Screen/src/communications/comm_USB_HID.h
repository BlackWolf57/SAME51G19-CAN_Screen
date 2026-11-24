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

#define POWERSTATE_TESTING(Ptr)		(((Ptr) -> Power_State == SENSOR_POWER_STATE_UNDEFINED) || ((Ptr) -> Power_State == SENSOR_POWER_STATE_D4_POWER_OFF))
//#define FEATURE_TESTING(Col)		((POWERSTATE_TESTING(Col.feature_Ptr_) == false) && ((Col.feature_Ptr_) -> State == SENSOR_STATE_READY))
//#define FEATURE_TESTING(Col)		(POWERSTATE_TESTING(Col.feature_Ptr_) == false)

enum : uint8_t {
	Free_Collection,
	First_Collection,
	Second_Collection,
	Both_Collection
};

struct Report_Collection_t {
	union {
		struct {
			uint8_t First_Report: 1;
			uint8_t Second_Report: 1;
			__I uint8_t : 6;
		};
		uint8_t Report;
	};
	Report_Collection_t () : Report ( Free_Collection ) {}
};

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
		uint8_t Connection_Type;								// Taille:  1 Byte
		uint8_t Power_State;								// Taille:  1 Byte
		uint8_t Report_State;									// Taille:  1 Byte
		uint8_t State;									// Taille:  1 Byte
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
			uint8_t Event;								// Taille:  1 Byte
			uint8_t State;								// Taille:  1 Byte
			union {
				struct {
					int16_t Chip;						// Taille: *2 Bytes
					int16_t Box;						// Taille: *2 Bytes
				};								// Taille: *4 Bytes
				int16_t Temperatures [ 2 ];					// Taille: *4 Bytes
			};
		} HID_Device1_Collection1_Input_Data_t;						// Total:   6 Bytes

		typedef struct {
			uint8_t Event;								// Taille:  1 Byte
			uint8_t State;								// Taille:  1 Byte
			voltage_t voltages_;							// Taille: 16 Bytes
		} HID_Device1_Collection2_Input_Data_t;						// Taille: 18 Bytes

		typedef struct {
			features_t temperatures_features_;					// Taille: 10 Bytes
			features_t voltages_features_;						// Taille: 10 Bytes
		} HID_Device1_Features_Data_t;							// Total:  20 Bytes !
	#endif

	/* Device 2 - AquaComputer High-Flow 2 !						*/
	#if ENABLED(HIGHFLOW2_DEVICE)
		typedef struct {
			uint8_t Event;								// Taille:  1 Byte
			uint8_t State;								// Taille:  1 Byte
			int16_t Temperature;							// Taille:  2 Bytes
		} HID_Device2_Collection1_Input_Data_t;						// Total:   4 Bytes

		typedef struct {
			uint8_t Event;								// Taille:  1 Byte
			uint8_t State;								// Taille:  1 Byte
			high_flow_t high_flow_;							// Taille:  2 Bytes
		} HID_Device2_Collection2_Input_Data_t;						// Total:   4 Bytes

		typedef struct {
			features_t temperatures_features_;					// Taille: 10 Bytes
			struct {
				features_t flow_features_;					// Taille: 10 Bytes
				uint8_t Name [ 32 ];						// Taille: 32 Bytes
			} flow_struct_;								// Taille: 42 Bytes
		} HID_Device2_Features_Data_t;							// Total:  52 Bytes !
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
			//clock_config_t clock_config_;						// Taille:  1 Byte
			//uint8_t Winter;
			//int8_t GMT;
			//int8_t DayInputOffset;
			//int8_t DayOutputOffset;
			uint8_t Reset;								// Taille:  1 Byte
		} HID_Device3_Features_Data_t;							// Total:
	#endif
_Pragma("pack()")

typedef struct {
	features_t* feature_Ptr_ = nullptr;
	uint32_t Timing;

	void set_FeaturePtr ( void* _Ptr_ )				{ this -> feature_Ptr_ = ( features_t* ) _Ptr_; }
	void set_NewTiming ()						{ this -> Timing = millis (); }
	void set_NewTiming ( uint8_t _Offset_ )				{ this -> Timing = ( millis () + _Offset_ ); }
	uint32_t next_Timing ()						{ return ( this -> feature_Ptr_ == nullptr ? this -> Timing : ( this -> feature_Ptr_ -> Interval == 0 ? this -> Timing + 1000 : this -> feature_Ptr_ -> Interval + this -> Timing ) ); }
} timing_t;

typedef struct {
	timing_t _First_Collection;
	timing_t _Second_Collection;
	uint8_t _lastResult = Free_Collection;

	void set_FeaturesPtr ( void* _Ptr1_, void* _Ptr2_ ) {
		this -> _First_Collection.set_FeaturePtr ( _Ptr1_ );
		this -> _Second_Collection.set_FeaturePtr ( _Ptr2_ );
	}
	void set_Timings ( uint32_t _timing1_, uint32_t _timing2_ ) {
		this -> _First_Collection.Timing = _timing1_;
		this -> _Second_Collection.Timing = _timing2_;
	}
	void update_Timings () {
		this -> _First_Collection.Timing = millis ();
		this -> _Second_Collection.Timing = millis ();
	}
	uint8_t lastResult () const							{ return this -> _lastResult; }
	uint8_t operator () () {
		Report_Collection_t _timing;

		if ( ( this -> _First_Collection.feature_Ptr_ == nullptr ) || ( this -> _Second_Collection.feature_Ptr_ == nullptr ) ) {
			this -> _lastResult = _timing.Report;
			return this -> _lastResult;
		}

		if ( POWERSTATE_TESTING ( this -> _First_Collection.feature_Ptr_ ) == false ) _timing.First_Report = ( millis () >= this -> _First_Collection.next_Timing () );
		if ( POWERSTATE_TESTING ( this -> _Second_Collection.feature_Ptr_ ) == false ) _timing.Second_Report = ( millis () >= this -> _Second_Collection.next_Timing () );

		if ( _timing.Report == Both_Collection ) {
			this -> _First_Collection.set_NewTiming ();
			this -> _Second_Collection.set_NewTiming ( 5 );
			_timing.Report = First_Collection;
		}
		else if ( _timing.Report == First_Collection ) this -> _First_Collection.set_NewTiming ();
		else if ( _timing.Report == Second_Collection ) this -> _Second_Collection.set_NewTiming ();

		this -> _lastResult = _timing.Report;
		return _lastResult;
	}
} scheduler_t;

typedef struct {
	#if ENABLED(SENSORS_DEVICE)
		scheduler_t Sensors_Scheduler;
	#endif
	#if ENABLED(HIGHFLOW2_DEVICE)
		scheduler_t HighFlow2_Scheduler;
	#endif
	//#if ENABLED(SCREEN_DEVICE)
	//	uint32_t _screen_timing;
	//#endif
} manager_t;



class CLASS_HID_ {
private:
	#if ENABLED(SENSORS_DEVICE)
		//HID_Device1_Input_Data_t sensors_interrupt_IN_;
		HID_Device1_Features_Data_t sensors_features_;
	#endif

	#if ENABLED(HIGHFLOW2_DEVICE)
		//HID_Device2_Input_Data_t highflow2_interrupt_IN_;
		HID_Device2_Features_Data_t highflow2_features_;
	#endif

	#if ENABLED(SCREEN_DEVICE)
		HID_Device3_Input_Data_t screen_interrupt_IN_;
		HID_Device3_Output_Data_t screen_interrupt_OUT_;
		HID_Device3_Features_Data_t screen_features_;
	#endif

	//packed_t _report_Packed;
	manager_t _manager;

	void loop_InputReport ();
	void loop_OutputReport ();
	void loop_SetReport ();

public:
	CLASS_HID_ ();
	void screen_feature_updated ();
	void begin ();
	void loop ();
};

extern CLASS_HID_ hid_;
