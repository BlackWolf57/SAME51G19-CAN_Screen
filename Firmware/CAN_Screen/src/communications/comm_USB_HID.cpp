#include "./comm_USB_HID.h"
#include "./HID_Report.h"
#include "../board/rtc.h"

CLASS_HID_::CLASS_HID_ () {
	#if ENABLED(SENSORS_DEVICE)
		static HIDSubDescriptor device1Node_ ( _Device1_HIDReportDescriptor, sizeof ( _Device1_HIDReportDescriptor ), SENSORS_DEVICE_NUMBER );
		customHID_ ().AppendDescriptor ( &device1Node_ );

		memset ( &this -> sensors_interrupt_IN_, 0, sizeof ( this -> sensors_interrupt_IN_ ) );

		this -> sensors_features_.temperatures_features_.Connection = SENSOR_CONNECTION_TYPE_PC_EXTERNAL;
		this -> sensors_features_.temperatures_features_.PowerState = SENSOR_POWER_STATE_D0_FULL_POWER;
		this -> sensors_features_.temperatures_features_.State = SENSOR_REPORTING_STATE_REPORT_ALL_EVENTS;
		this -> sensors_features_.temperatures_features_.Status = SENSOR_STATE_READY;
		this -> sensors_features_.temperatures_features_.Interval = 1000;			// Intervale entre deux reports ( en millisecondes )
		this -> sensors_features_.temperatures_features_.Sensivity = 10;
		this -> _sensors_temperature_timing = 0;

		this -> sensors_features_.voltages_features_.Connection = SENSOR_CONNECTION_TYPE_PC_EXTERNAL;
		this -> sensors_features_.voltages_features_.PowerState = SENSOR_POWER_STATE_D0_FULL_POWER;
		this -> sensors_features_.voltages_features_.State = SENSOR_REPORTING_STATE_REPORT_ALL_EVENTS;
		this -> sensors_features_.voltages_features_.Status = SENSOR_STATE_READY;
		this -> sensors_features_.voltages_features_.Interval = 1000;
		this -> sensors_features_.voltages_features_.Sensivity = 10;
		this -> _sensors_monitor_timing = 0;
	#endif
	#if ENABLED(HIGHFLOW2_DEVICE)
		static HIDSubDescriptor device2Node_ ( _Device2_HIDReportDescriptor, sizeof ( _Device2_HIDReportDescriptor ), HIGHFLOW2_DEVICE_NUMBER );
		customHID_ ().AppendDescriptor ( &device2Node_ );

		memset ( &this -> highflow2_interrupt_IN_, 0, sizeof ( this -> highflow2_interrupt_IN_ ) );

		this -> highflow2_features_.temperatures_features_.Connection = SENSOR_CONNECTION_TYPE_PC_EXTERNAL;
		this -> highflow2_features_.temperatures_features_.PowerState = SENSOR_POWER_STATE_D0_FULL_POWER;
		this -> highflow2_features_.temperatures_features_.State = SENSOR_REPORTING_STATE_REPORT_ALL_EVENTS;
		this -> highflow2_features_.temperatures_features_.Status = SENSOR_STATE_READY;
		this -> highflow2_features_.temperatures_features_.Interval = 1000;
		this -> highflow2_features_.temperatures_features_.Sensivity = 10;
		this -> _highflow2_temperature_timing = 0;

		this -> highflow2_features_.flow_features_.Connection = SENSOR_CONNECTION_TYPE_PC_EXTERNAL;
		this -> highflow2_features_.flow_features_.PowerState = SENSOR_POWER_STATE_D0_FULL_POWER;
		this -> highflow2_features_.flow_features_.State = SENSOR_REPORTING_STATE_REPORT_ALL_EVENTS;
		this -> highflow2_features_.flow_features_.Status = SENSOR_STATE_READY;
		this -> highflow2_features_.flow_features_.Interval = 1000;
		this -> highflow2_features_.flow_features_.Sensivity = 10;
		this -> _highflow2_flow_timing = 0;
	#endif
	#if ENABLED(SCREEN_DEVICE)
		static HIDSubDescriptor device3Node_ ( _Device3_HIDReportDescriptor, sizeof ( _Device3_HIDReportDescriptor ), SCREEN_DEVICE_NUMBER );
		customHID_ ().AppendDescriptor ( &device3Node_ );

		memset ( &this -> screen_interrupt_IN_, 0, sizeof ( this -> screen_interrupt_IN_ ) );
		memset ( &this -> screen_interrupt_OUT_, 0, sizeof ( this -> screen_interrupt_OUT_ ) );

		this -> screen_features_.firmware_.Firmware_Minor = FIRMWARE_NUMBER & 0xFF;
		this -> screen_features_.firmware_.Firmware_Major = ( FIRMWARE_NUMBER & 0xFF00 ) >> 8;
		this -> screen_features_.Timestamp = 0;
		this -> screen_features_.clock_config_.Mode_24h = ( ( ampm_t ) rtc_.am_pm () == hour_24 );
		this -> screen_features_.clock_config_.Mode_AutoGMT = rtc_.get_AutoWinter ();
		this -> screen_features_.clock_config_.Clock = ( ( clock_view_t ) screen_.Display () ) & 0x3;
		this -> _screen_timing = 0;
	#endif

	memset ( &this -> _report_Packed, 0, sizeof ( this -> _report_Packed ) );
}

/** @brief			Routine de gestion des données sortantes.
 *  @details			Routine de gestion des données sortantes. DEVICE_TO_HOST => SendReport
 */
void CLASS_HID_::loop_InputReport () {
	#if ENABLED(DEBUG)
		static uint8_t _debug_printed = 0;
	#endif

	if ( ( time_.seconds > 10 ) && ( customHID_ ().is_Plugged () ) ) {
		#if ENABLED(SENSORS_DEVICE)
			if ( ( ( millis () - this -> _sensors_temperature_timing ) > this -> sensors_features_.temperatures_features_.Interval ) && ( this -> sensors_features_.temperatures_features_.PowerState == SENSOR_POWER_STATE_D0_FULL_POWER ) && ( this -> _report_Packed.Sensors_TemperatureDevice ) ) {
				if ( customHID_ ().isReadyToTransmit ( SENSORS_HID_ENDPOINT_NUMBER ) ) {
					customHID_ ().sendReport ( Sensors_TemperatureReport_ID, &this -> sensors_interrupt_IN_.Temperatures, sizeof ( this -> sensors_interrupt_IN_.Temperatures ), SENSORS_DEVICE_NUMBER );
					//customHID_ ().sendReport ( TemperatureInput_ID, &this -> sensors_interrupt_IN_.Temperatures, sizeof ( this -> sensors_interrupt_IN_.Temperatures ), SENSORS_DEVICE_NUMBER );
					//customHID_ ().sendReport ( TemperatureInput_ID, &this -> sensors_interrupt_IN_.hid_temps_, sizeof ( this -> sensors_interrupt_IN_.hid_temps_ ), SENSORS_DEVICE_NUMBER );

					this -> _report_Packed.Sensors_TemperatureDevice = false;
					this -> _sensors_temperature_timing = millis ();
				}
			}
			else {
				if ( this -> _report_Packed.Sensors_TemperatureDevice == false ) {
					temperatures_t _tmpData;
					temperatures_.get ( &_tmpData );

					this -> sensors_interrupt_IN_.Chip = _tmpData.Convert ( _tmpData.Chip, deg_Celsius );
					this -> sensors_interrupt_IN_.Box = _tmpData.Convert ( _tmpData.Box, deg_Celsius );
					//this -> sensors_interrupt_IN_.hid_temps_.Status = SENSOR_STATE_READY;
					//this -> sensors_interrupt_IN_.hid_temps_.Event = SENSOR_EVENT_DATA_UPDATED;
					//this -> sensors_interrupt_IN_.hid_temps_.Chip = _tmpData.Convert ( _tmpData.Chip, deg_Celsius );
					//this -> sensors_interrupt_IN_.hid_temps_.Box = _tmpData.Convert ( _tmpData.Box, deg_Celsius );

					this -> _report_Packed.Sensors_TemperatureDevice = true;
				}
			}

			if ( ( ( millis () - this -> _sensors_monitor_timing ) > this -> sensors_features_.voltages_features_.Interval ) && ( this -> sensors_features_.voltages_features_.PowerState == SENSOR_POWER_STATE_D0_FULL_POWER ) && ( this -> _report_Packed.Sensors_MonitorDevice ) ) {
				if ( customHID_ ().isReadyToTransmit ( SENSORS_HID_ENDPOINT_NUMBER ) ) {
					customHID_ ().sendReport ( Sensors_MonitorReport_ID, &this -> sensors_interrupt_IN_.voltages_, sizeof ( this -> sensors_interrupt_IN_.voltages_ ), SENSORS_DEVICE_NUMBER );
					//customHID_ ().sendReport ( MonitorInput_ID, &this -> sensors_interrupt_IN_.voltages_, sizeof ( this -> sensors_interrupt_IN_.voltages_ ), SENSORS_DEVICE_NUMBER );
					//customHID_ ().sendReport ( MonitorInput_ID, &this -> sensors_interrupt_IN_.hid_monitor_, sizeof ( this -> sensors_interrupt_IN_.hid_monitor_ ), SENSORS_DEVICE_NUMBER );

					this -> _report_Packed.Sensors_MonitorDevice = false;
					this -> _sensors_monitor_timing = millis ();
				}
			}
			else {
				if ( this -> _report_Packed.Sensors_MonitorDevice == false ) {
					monitor_.get ( &this -> sensors_interrupt_IN_.voltages_ );
					//this -> sensors_interrupt_IN_.hid_monitor_.Status = SENSOR_STATE_READY;
					//this -> sensors_interrupt_IN_.hid_monitor_.Event = SENSOR_EVENT_DATA_UPDATED;
					//monitor_.get ( &this -> sensors_interrupt_IN_.hid_monitor_.voltages_ );

					this -> _report_Packed.Sensors_MonitorDevice = true;
				}
			}
		#endif

		#if ENABLED(HIGHFLOW2_DEVICE)
			if ( ( ( millis () - this -> _highflow2_temperature_timing ) > this -> highflow2_features_.temperatures_features_.Interval ) && ( this -> highflow2_features_.temperatures_features_.PowerState == SENSOR_POWER_STATE_D0_FULL_POWER ) && ( this -> _report_Packed.HighFlow2_TemperatureDevice ) ) {
				customHID_ ().sendReport ( HighFlow2_TemperatureReport_ID, &this -> highflow2_interrupt_IN_.Temperature, sizeof ( this -> highflow2_interrupt_IN_.Temperature ), HIGHFLOW2_DEVICE_NUMBER );

				this -> _report_Packed.HighFlow2_TemperatureDevice = false;
				this -> _highflow2_temperature_timing = millis ();
			}
			else {
				if ( this -> _report_Packed.HighFlow2_TemperatureDevice == false ) {
					temperatures_t _tmpData;
					temperatures_.get ( &_tmpData );

					this -> highflow2_interrupt_IN_.Temperature = _tmpData.Convert ( _tmpData.Water, deg_Celsius );
					this -> _report_Packed.HighFlow2_TemperatureDevice = true;
				}
			}

			if ( ( ( millis () - this -> _highflow2_flow_timing ) > this -> highflow2_features_.flow_features_.Interval ) && ( this -> highflow2_features_.flow_features_.PowerState == SENSOR_POWER_STATE_D0_FULL_POWER ) && ( this -> _report_Packed.HighFlow2_FlowDevice ) ) {
				customHID_ ().sendReport ( HighFlow2_FlowReport_ID, &this -> highflow2_interrupt_IN_.high_flow_, sizeof ( this -> highflow2_interrupt_IN_.high_flow_ ), HIGHFLOW2_DEVICE_NUMBER );

				this -> _report_Packed.HighFlow2_FlowDevice = false;
				this -> _highflow2_flow_timing = millis ();
			}
			else {
				if ( this -> _report_Packed.HighFlow2_FlowDevice == false ) {
					debimetre_.get ( &this -> highflow2_interrupt_IN_.high_flow_ );
					this -> _report_Packed.HighFlow2_FlowDevice = true;
				}
			}
		#endif

		#if ENABLED(SCREEN_DEVICE)
			uint16_t _screen_idle = customHID_ ().get_Idle ( SCREEN_DEVICE_NUMBER ) << 2;

			if ( _screen_idle > 0 ) {
				if ( ( ( millis () - this -> _screen_timing ) > _screen_idle ) && ( this -> _report_Packed.Screen_Device ) ) {
					customHID_ ().sendReport ( &this -> screen_interrupt_IN_, sizeof ( this -> screen_interrupt_IN_ ), SCREEN_DEVICE_NUMBER );

					this -> _report_Packed.Screen_Device = false;
					this -> _screen_timing = millis ();
				}
				else {
					if ( this -> _report_Packed.Screen_Device == false ) {

						this -> _report_Packed.Screen_Device = true;
					}
				}
				#if ENABLED(DEBUG)
					if ( bitRead ( _debug_printed, 2 ) ) bitClear ( _debug_printed, 2 );
				#endif
			}
			#if ENABLED(DEBUG)
				else {									// Idle = 0 !
					if ( bitRead ( _debug_printed, 2 ) == false ) {
						SerialUSB.println ( "Screen device - Idle infini !" );
						bitSet ( _debug_printed, 2 );
					}
				}
			#endif
		#endif
	}
}

/** @brief			Routine de gestion des données entrantes.
 *  @details			Routine de gestion des données entrantes. REQUEST_HOSTTODEVICE_CLASS_INTERFACE => HID_SET_REPORT ( HID_OUTPUT_REPORT_TYPE )
 */
void CLASS_HID_::loop_OutputReport () {
}

/** @brief			Routine de gestion des données entrantes ( FEATURES ).
 *  @details			Routine de gestion de configuration. REQUEST_HOSTTODEVICE_CLASS_INTERFACE => HID_SET_REPORT ( HID_FEATURE_REPORT_TYPE )
 */
void CLASS_HID_::loop_SetReport () {
	uint8_t _tmpType = 0;
	uint8_t _ID = 0;
	uint8_t _interface_Number = 0;
	uint8_t _tmpData [ EP0_SIZE ];
	memset ( _tmpData, 0, EP0_SIZE );

	customHID_ ().getReport ( &_tmpType, &_ID, &_interface_Number, _tmpData );
	switch ( _interface_Number ) {
		#if ENABLED(SENSORS_DEVICE)
		case SENSORS_HID_INTERFACE_NUMBER:
			switch ( _tmpType ) {
				case HID_INPUT_REPORT_TYPE:
					SerialUSB.println ( "Set Report de type INPUT sur l'interface SENSOR_DEVICE." );
					break;
				case HID_OUTPUT_REPORT_TYPE:
					SerialUSB.println ( "Set Report de type OUTPUT sur l'interface SENSOR_DEVICE." );
					break;
				case HID_FEATURE_REPORT_TYPE:
					switch ( _ID ) {
						case TemperatureFeature_ID:
						case Sensors_TemperatureReport_ID:
							memcpy ( &this -> sensors_features_.temperatures_features_, _tmpData, sizeof ( this -> sensors_features_.temperatures_features_ ) );
							break;
						case MonitorFeature_ID:
						case Sensors_MonitorReport_ID:
							memcpy ( &this -> sensors_features_.voltages_features_, _tmpData, sizeof ( this -> sensors_features_.voltages_features_ ) );
							break;
					}
					break;
			}
			break;
		#endif
		#if ENABLED(HIGHFLOW2_DEVICE)
		case HIGHFLOW2_HID_INTERFACE_NUMBER:
			switch ( _tmpType ) {
				case HID_INPUT_REPORT_TYPE:
					SerialUSB.println ( "Set Report de type INPUT sur l'interface SENSOR_DEVICE." );
					break;
				case HID_OUTPUT_REPORT_TYPE:
					SerialUSB.println ( "Set Report de type OUTPUT sur l'interface SENSOR_DEVICE." );
					break;
				case HID_FEATURE_REPORT_TYPE:
					switch ( _ID ) {
						case HighFlow2_TemperatureReport_ID:
							memcpy ( &this -> highflow2_features_.temperatures_features_, _tmpData, sizeof ( this -> highflow2_features_.temperatures_features_ ) );
							break;
						case HighFlow2_FlowReport_ID:
							memcpy ( &this -> highflow2_features_.flow_features_, _tmpData, sizeof ( this -> highflow2_features_.flow_features_ ) );
							break;
					}
					break;
			}
			break;
		#endif
		#if ENABLED(SCREEN_DEVICE)
		case SCREEN_HID_INTERFACE_NUMBER:
			break;
		#endif
	}
	/*uint16_t _length = customHID_ ().getReport ( &_tmpType, &_ID, _tmpData );
	if ( _length > 0 ) {
		SerialUSB.print ( "Set Report Request !\t\twValueH = 0x0" );
		SerialUSB.print ( _tmpType, HEX );
		switch ( _tmpType ) {
			case HID_INPUT_REPORT_TYPE:
				SerialUSB.print ( " (INPUT)\t\twValueL = " );
				break;
			case HID_OUTPUT_REPORT_TYPE:
				SerialUSB.print ( " (OUTPUT)\t\twValueL = " );
				break;
			case HID_FEATURE_REPORT_TYPE:
				SerialUSB.print ( " (FEATURE)\t\twValueL = " );
				break;
			default:
				SerialUSB.print ( "\t\twValueL = " );
				break;
		}
		SerialUSB.print ( _ID < 0x10 ? "0x0" : "0x" );
		SerialUSB.print ( _ID, HEX );
		SerialUSB.print ( "\t\tbLength = " );
		SerialUSB.println ( _length, DEC );

		if ( _tmpType == HID_FEATURE_REPORT_TYPE ) {
			SerialUSB.print ( "sizeof ( _tmpData ) = " );
			SerialUSB.print ( sizeof ( _tmpData ), DEC );
			SerialUSB.print ( ( sizeof ( _tmpData ) < sizeof ( this -> feature_ ) ) ?
					"\t\t/!\\ sizeof ( _tmpData ) < sizeof ( this -> feature_ ) !\n\r" :
					( sizeof ( _tmpData ) > sizeof ( this -> feature_ ) ) ?
							"\t\t/!\\ sizeof ( _tmpData ) > sizeof ( this -> feature_ ) !\n\r" :
							"\n\r" );
			SerialUSB.print ( "Data: " );
			for ( uint32_t _pos = 0; _pos < _length; _pos ++ ) {
				SerialUSB.print ( _tmpData [ _pos ] < 0x10 ? "0x0" : "0x" );
				SerialUSB.print ( _tmpData [ _pos ], HEX );
				SerialUSB.print ( _pos < ( _length - 1 ) ? ", " : "\t\tLength = " );
			}
			SerialUSB.println ( _length, DEC );

			customHID_ ().clearReport ();
			//HID_feature_Data_t input_Feature_;
			//memcpy( &input_Feature_, &_tmpData, sizeof ( input_Feature_ ) );
			memcpy ( &this -> feature_, &_tmpData, sizeof ( this -> feature_ ) );
		}
	}*/
}

void CLASS_HID_::begin () {
	#if ENABLED(SENSORS_DEVICE)
		this -> sensors_features_.temperatures_features_.Interval = temperatures_.Interval ();
		this -> sensors_features_.voltages_features_.Interval = monitor_.Interval ();
		//customHID_ ().AppendFeature ( feature_Temperature, &this -> sensors_features_.temperatures_features_, sizeof ( this -> sensors_features_.temperatures_features_ ), TemperatureFeature_ID );
		//customHID_ ().AppendFeature ( feature_Monitor, &this -> sensors_features_.voltages_features_, sizeof ( this -> sensors_features_.voltages_features_ ), MonitorFeature_ID );
		customHID_ ().AppendFeature ( feature_SensorsTemperature, &this -> sensors_features_.temperatures_features_, sizeof ( this -> sensors_features_.temperatures_features_ ), Sensors_TemperatureReport_ID );
		customHID_ ().AppendFeature ( feature_SensorsMonitor, &this -> sensors_features_.voltages_features_, sizeof ( this -> sensors_features_.voltages_features_ ), Sensors_MonitorReport_ID );
	#endif
	#if ENABLED(HIGHFLOW2_DEVICE)
		this -> highflow2_features_.temperatures_features_.Interval = temperatures_.Interval ();
		customHID_ ().AppendFeature ( feature_HighFlow2Temperature, &this -> highflow2_features_.temperatures_features_, sizeof ( this -> highflow2_features_.temperatures_features_ ), HighFlow2_TemperatureReport_ID );
		customHID_ ().AppendFeature ( feature_HighFlow2Flow, &this -> highflow2_features_.flow_features_, sizeof ( this -> highflow2_features_.flow_features_ ), HighFlow2_FlowReport_ID );
	#endif
	#if ENABLED(SCREEN_DEVICE)
		customHID_ ().AppendFeature ( 0, ( uint32_t* ) &this -> screen_features_, sizeof ( this -> screen_features_ ), 0 );
	#endif
}

void CLASS_HID_::loop () {
	this -> loop_InputReport ();						// INPUT Part !			Données entrantes sur le serveur.			( Ecran => PC )
	this -> loop_OutputReport ();						// OUTPUT Part !		Données sortantes du serveur.				( PC => Ecran )
	this -> loop_SetReport ();						// FEATURE Part !		Paramétrage sortant du serveur.				( PC => Ecran )
}

CLASS_HID_ hid_;
