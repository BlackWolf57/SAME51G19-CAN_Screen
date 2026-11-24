#include "./comm_USB_HID.h"
#include "./HID_Report.h"
#include "../board/rtc.h"
#include "../USB/HID_Strings.h"

CLASS_HID_::CLASS_HID_ () {
	#if ENABLED(SENSORS_DEVICE)
		static HIDSubDescriptor device1Node_ ( _Device1_HIDReportDescriptor, sizeof ( _Device1_HIDReportDescriptor ), SENSORS_DEVICE_NUMBER );
		customHID_ ().AppendDescriptor ( &device1Node_ );

		this -> sensors_features_.temperatures_features_.Connection_Type = SENSOR_CONNECTION_TYPE_PC_EXTERNAL;
		this -> sensors_features_.temperatures_features_.Power_State = SENSOR_POWER_STATE_D4_POWER_OFF;
		this -> sensors_features_.temperatures_features_.Report_State = SENSOR_REPORTING_STATE_REPORT_ALL_EVENTS;
		this -> sensors_features_.temperatures_features_.State = SENSOR_STATE_INITIALIZING;
		this -> sensors_features_.temperatures_features_.Interval = 1000;			// Intervalle entre deux reports ( en millisecondes )
		this -> sensors_features_.temperatures_features_.Sensivity = 10;

		this -> sensors_features_.voltages_features_.Connection_Type = SENSOR_CONNECTION_TYPE_PC_EXTERNAL;
		this -> sensors_features_.voltages_features_.Power_State = SENSOR_POWER_STATE_D4_POWER_OFF;
		this -> sensors_features_.voltages_features_.Report_State = SENSOR_REPORTING_STATE_REPORT_ALL_EVENTS;
		this -> sensors_features_.voltages_features_.State = SENSOR_STATE_INITIALIZING;
		this -> sensors_features_.voltages_features_.Interval = 1000;
		this -> sensors_features_.voltages_features_.Sensivity = 10;

		//memset ( &this -> sensors_interrupt_IN_, 0, sizeof ( this -> sensors_interrupt_IN_ ) );
		this -> _manager.Sensors_Scheduler.set_Timings ( 0, 0 );
		this -> _manager.Sensors_Scheduler.set_FeaturesPtr ( &this -> sensors_features_.temperatures_features_, &this -> sensors_features_.voltages_features_ );
	#endif
	#if ENABLED(HIGHFLOW2_DEVICE)
		static HIDSubDescriptor device2Node_ ( _Device2_HIDReportDescriptor, sizeof ( _Device2_HIDReportDescriptor ), HIGHFLOW2_DEVICE_NUMBER );
		customHID_ ().AppendDescriptor ( &device2Node_ );

		this -> highflow2_features_.temperatures_features_.Connection_Type = SENSOR_CONNECTION_TYPE_PC_EXTERNAL;
		this -> highflow2_features_.temperatures_features_.Power_State = SENSOR_POWER_STATE_D4_POWER_OFF;
		this -> highflow2_features_.temperatures_features_.Report_State = SENSOR_REPORTING_STATE_REPORT_ALL_EVENTS;
		this -> highflow2_features_.temperatures_features_.State = SENSOR_STATE_INITIALIZING;
		this -> highflow2_features_.temperatures_features_.Interval = 1000;
		this -> highflow2_features_.temperatures_features_.Sensivity = 10;

		this -> highflow2_features_.flow_struct_.flow_features_.Connection_Type = SENSOR_CONNECTION_TYPE_PC_EXTERNAL;
		this -> highflow2_features_.flow_struct_.flow_features_.Power_State = SENSOR_POWER_STATE_D4_POWER_OFF;
		this -> highflow2_features_.flow_struct_.flow_features_.Report_State = SENSOR_REPORTING_STATE_REPORT_ALL_EVENTS;
		this -> highflow2_features_.flow_struct_.flow_features_.State = SENSOR_STATE_INITIALIZING;
		this -> highflow2_features_.flow_struct_.flow_features_.Interval = 1000;
		this -> highflow2_features_.flow_struct_.flow_features_.Sensivity = 10;
		memcpy ( &this -> highflow2_features_.flow_struct_.Name, CAN_SCREEN_HIGHFLOW2_FLOW_UTF16, sizeof ( CAN_SCREEN_HIGHFLOW2_FLOW_UTF16 ) );

		//memset ( &this -> highflow2_interrupt_IN_, 0, sizeof ( this -> highflow2_interrupt_IN_ ) );
		this -> _manager.HighFlow2_Scheduler.set_Timings ( 0, 0 );
		this -> _manager.HighFlow2_Scheduler.set_FeaturesPtr ( &this -> highflow2_features_.temperatures_features_, &this -> highflow2_features_.flow_struct_.flow_features_ );
	#endif
	#if ENABLED(SCREEN_DEVICE)
		static HIDSubDescriptor device3Node_ ( _Device3_HIDReportDescriptor, sizeof ( _Device3_HIDReportDescriptor ), SCREEN_DEVICE_NUMBER );
		customHID_ ().AppendDescriptor ( &device3Node_ );

		memset ( &this -> screen_interrupt_IN_, 0, sizeof ( this -> screen_interrupt_IN_ ) );
		memset ( &this -> screen_interrupt_OUT_, 0, sizeof ( this -> screen_interrupt_OUT_ ) );

		this -> screen_features_.firmware_.Firmware_Minor = FIRMWARE_NUMBER & 0xFF;
		this -> screen_features_.firmware_.Firmware_Major = ( FIRMWARE_NUMBER & 0xFF00 ) >> 8;
		this -> screen_features_.Timestamp = 0;
		//this -> screen_features_.clock_config_.Mode_24h = ( ( ampm_t ) rtc_.am_pm () == hour_24 );
		//this -> screen_features_.clock_config_.Mode_AutoGMT = rtc_.get_AutoWinter ();
		//this -> screen_features_.clock_config_.Clock = ( ( clock_view_t ) screen_.Display () ) & 0x3;
		this -> screen_features_.Reset = 0;
		//this -> _manager._screen_timing = 0;
	#endif

	//memset ( &this -> _report_Packed, 0, sizeof ( this -> _report_Packed ) );
}

/** @brief			Routine de gestion des données sortantes.
 *  @details			Routine de gestion des données sortantes. DEVICE_TO_HOST => SendReport
 */
void CLASS_HID_::loop_InputReport () {
	if ( ( time_.seconds > 10 ) && ( customHID_ ().is_Plugged () ) ) {
		#if ENABLED(SENSORS_DEVICE)
			uint8_t _sensors_CollectionToTransmit = this -> _manager.Sensors_Scheduler ();

			if ( _sensors_CollectionToTransmit == First_Collection ) {
				SerialUSB.println ( "Testing First Collection..." );
				if ( customHID_ ().isReadyToTransmit ( SENSORS_HID_ENDPOINT_NUMBER ) ) {
					SerialUSB.println ( "Packing Sensors Device Temperature Collection" );
					HID_Device1_Collection1_Input_Data_t _tmpReport;
					temperatures_t _tmpData;
					temperatures_.get ( &_tmpData );

					_tmpReport.Event = SENSOR_EVENT_DATA_UPDATED;
					_tmpReport.State = this -> sensors_features_.temperatures_features_.State;
					_tmpReport.Chip = _tmpData.Convert ( _tmpData.Chip, deg_Celsius );
					_tmpReport.Box = _tmpData.Convert ( _tmpData.Box, deg_Celsius );

					SerialUSB.println ( "Sending First Collection report !" );
					customHID_ ().sendReport ( Sensors_TemperatureReport_ID, &_tmpReport, sizeof ( _tmpReport ), SENSORS_DEVICE_NUMBER );
				}
			}

			if ( _sensors_CollectionToTransmit == Second_Collection ) {
				if ( customHID_ ().isReadyToTransmit ( SENSORS_HID_ENDPOINT_NUMBER ) ) {
					HID_Device1_Collection2_Input_Data_t _tmpReport;
					monitor_.get ( &_tmpReport.voltages_ );
					_tmpReport.Event = SENSOR_EVENT_DATA_UPDATED;
					_tmpReport.State = this -> sensors_features_.voltages_features_.State;

					customHID_ ().sendReport ( Sensors_MonitorReport_ID, &_tmpReport, sizeof ( _tmpReport ), SENSORS_DEVICE_NUMBER );
				}
			}
		#endif

		#if ENABLED(HIGHFLOW2_DEVICE)
			uint8_t _highflow2_CollectionToTransmit = this -> _manager.HighFlow2_Scheduler ();

			if ( _highflow2_CollectionToTransmit == First_Collection ) {
				if ( customHID_ ().isReadyToTransmit ( HIGHFLOW2_HID_ENDPOINT_NUMBER ) ) {
					HID_Device2_Collection1_Input_Data_t _tmpReport;
					temperatures_t _tmpData;
					temperatures_.get ( & _tmpData );

					_tmpReport.Event = SENSOR_EVENT_DATA_UPDATED;
					_tmpReport.State = this -> highflow2_features_.temperatures_features_.State;
					_tmpReport.Temperature = _tmpData.Convert ( _tmpData.Water, deg_Celsius );

					customHID_ ().sendReport ( HighFlow2_TemperatureReport_ID, &_tmpReport, sizeof ( _tmpReport ), HIGHFLOW2_DEVICE_NUMBER );
				}
			}

			if ( _highflow2_CollectionToTransmit == Second_Collection ) {
				if ( customHID_ ().isReadyToTransmit ( HIGHFLOW2_HID_ENDPOINT_NUMBER ) ) {
					HID_Device2_Collection2_Input_Data_t _tmpReport;
					debimetre_.get ( &_tmpReport.high_flow_ );
					_tmpReport.Event = SENSOR_EVENT_DATA_UPDATED;
					_tmpReport.State = this -> highflow2_features_.flow_struct_.flow_features_.State;

					customHID_ ().sendReport ( HighFlow2_FlowReport_ID, &_tmpReport, sizeof ( _tmpReport ), HIGHFLOW2_DEVICE_NUMBER );
				}
			}
		#endif

		/*#if ENABLED(SCREEN_DEVICE)
			uint16_t _screen_idle = customHID_ ().get_Idle ( SCREEN_DEVICE_NUMBER ) << 2;

			if ( _screen_idle > 0 ) {
				if ( ( ( millis () - this -> _manager._screen_timing ) > _screen_idle ) && ( this -> _report_Packed.Screen_Device ) ) {
					customHID_ ().sendReport ( &this -> screen_interrupt_IN_, sizeof ( this -> screen_interrupt_IN_ ), SCREEN_DEVICE_NUMBER );

					this -> _report_Packed.Screen_Device = false;
					this -> _screen_timing = millis ();
				}
				else {
					if ( this -> _report_Packed.Screen_Device == false ) {

						this -> _report_Packed.Screen_Device = true;
					}
				}
			}
		#endif*/
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
						case Sensors_TemperatureReport_ID:
							//SerialUSB.print ( "loop_SetReport: Lecture de: " );
							//lazy_print ( _tmpData, sizeof ( _tmpData ) );
							memcpy ( &this -> sensors_features_.temperatures_features_, _tmpData, sizeof ( this -> sensors_features_.temperatures_features_ ) );
							//SerialUSB.print ( "loop_SetReport: Copie de: " );
							//lazy_print ( &this -> sensors_features_.temperatures_features_, sizeof ( this -> sensors_features_.temperatures_features_ ) );
							//if ( ( ( this -> sensors_features_.temperatures_features_.Interval >> 24 ) & 0xFF ) == 0x5F ) this -> sensors_features_.temperatures_features_.Interval &= 0xFFFFFF;
							this -> _manager.Sensors_Scheduler.update_Timings ();
							break;
						case Sensors_MonitorReport_ID:
							memcpy ( &this -> sensors_features_.voltages_features_, _tmpData, sizeof ( this -> sensors_features_.voltages_features_ ) );
							//if ( ( ( this -> sensors_features_.voltages_features_.Interval >> 24 ) & 0xFF ) == 0x5F ) this -> sensors_features_.voltages_features_.Interval &= 0xFFFFFF;
							this -> _manager.Sensors_Scheduler.update_Timings ();
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
							//if ( ( ( this -> highflow2_features_.temperatures_features_.Interval >> 24 ) & 0xFF ) == 0x5F ) this -> highflow2_features_.temperatures_features_.Interval &= 0xFFFFFF;
							this -> _manager.HighFlow2_Scheduler.update_Timings ();
							break;
						case HighFlow2_FlowReport_ID:
							memcpy ( &this -> highflow2_features_.flow_struct_.flow_features_, _tmpData, sizeof ( this -> highflow2_features_.flow_struct_.flow_features_ ) );
							//if ( ( ( this -> highflow2_features_.flow_struct_.flow_features_.Interval >> 24 ) & 0xFF ) == 0x5F ) this -> highflow2_features_.flow_struct_.flow_features_.Interval &= 0xFFFFFF;
							this -> _manager.HighFlow2_Scheduler.update_Timings ();
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
}

void CLASS_HID_::screen_feature_updated () {
	if ( customHID_ ().isReadyToTransmit ( SCREEN_HID_IN_ENDPOINT_NUMBER ) ) {
		customHID_ ().sendReport ( &this -> screen_interrupt_IN_, sizeof ( this -> screen_interrupt_IN_ ), SCREEN_DEVICE_NUMBER );
	}
}

void CLASS_HID_::begin () {
	#if ENABLED(SENSORS_DEVICE)
		this -> sensors_features_.temperatures_features_.Interval = temperatures_.Interval ();
		this -> sensors_features_.voltages_features_.Interval = monitor_.Interval ();
		customHID_ ().AppendFeature ( feature_SensorsTemperature, &this -> sensors_features_.temperatures_features_, sizeof ( this -> sensors_features_.temperatures_features_ ), Sensors_TemperatureReport_ID );
		customHID_ ().AppendFeature ( feature_SensorsMonitor, &this -> sensors_features_.voltages_features_, sizeof ( this -> sensors_features_.voltages_features_ ), Sensors_MonitorReport_ID );
	#endif
	#if ENABLED(HIGHFLOW2_DEVICE)
		this -> highflow2_features_.temperatures_features_.Interval = temperatures_.Interval ();
		customHID_ ().AppendFeature ( feature_HighFlow2Temperature, &this -> highflow2_features_.temperatures_features_, sizeof ( this -> highflow2_features_.temperatures_features_ ), HighFlow2_TemperatureReport_ID );
		customHID_ ().AppendFeature ( feature_HighFlow2Flow, &this -> highflow2_features_.flow_struct_, sizeof ( this -> highflow2_features_.flow_struct_ ), HighFlow2_FlowReport_ID );
	#endif
	#if ENABLED(SCREEN_DEVICE)
		customHID_ ().AppendFeature ( feature_Firmware, ( uint32_t* ) &this -> screen_features_.firmware_, sizeof ( this -> screen_features_.firmware_ ), Firmware_ID );
		customHID_ ().AppendFeature ( feature_TimeStamp, ( uint32_t* ) &this -> screen_features_.firmware_, sizeof ( this -> screen_features_.firmware_ ), TimeStamp_ID );
		customHID_ ().AppendFeature ( feature_Reset, ( uint32_t* ) &this -> screen_features_.firmware_, sizeof ( this -> screen_features_.firmware_ ), Reset_ID );
	#endif
}

void CLASS_HID_::loop () {
	this -> loop_InputReport ();						// INPUT Part !			Données entrantes sur le serveur.			( Ecran => PC )
	this -> loop_OutputReport ();						// OUTPUT Part !		Données sortantes du serveur.				( PC => Ecran )
	this -> loop_SetReport ();						// FEATURE Part !		Paramétrage sortant du serveur.				( PC => Ecran )
}

CLASS_HID_ hid_;
