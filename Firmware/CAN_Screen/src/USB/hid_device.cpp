#include "./hid_device.h"
#include "./HID_Strings.h"
#include "../../configuration.h"
#include "USB/USBCore.cpp"

extern USBDevice_ usbd;;

#if defined(USBCON)
	CLASS_CUSTOM_HID_& customHID_ () {
		static CLASS_CUSTOM_HID_ obj_;
		return obj_;
	}

	CLASS_CUSTOM_HID_::CLASS_CUSTOM_HID_ () : PluggableUSBModule ( DEVICE_HID_ENDPOINT_COUNT, DEVICE_HID_INTERFACE_COUNT, this -> _epType.EndpointType ) {
		this -> globalNode_ = NULL;
		#if ENABLED(SENSORS_DEVICE)
			this -> _DescSize.Sensors_DescriptorSize = 0;
			this -> _Protocol.Sensors_Protocol = HID_REPORT_PROTOCOL;
			this -> _Idle.Sensors_Idle = 125;				// 125 x 4mS = 500mS
			this -> _epType.Sensors_Endpoint_In = USB_ENDPOINT_TYPE_INTERRUPT | USB_ENDPOINT_IN ( 0 );
		#endif

		#if ENABLED(HIGHFLOW2_DEVICE)
			this -> _DescSize.HighFlow2_DescriptorSize = 0;
			this -> _Protocol.HighFlow2_Protocol = HID_REPORT_PROTOCOL;
			this -> _Idle.HighFlow2_Idle = 250;				// 250 x 4mS = 1S
			this -> _epType.HighFlow2_Endpoint_In = USB_ENDPOINT_TYPE_INTERRUPT | USB_ENDPOINT_IN ( 0 );
		#endif

		#if ENABLED(SCREEN_DEVICE)
			this -> _DescSize.Screen_DescriptorSize = 0;
			this -> _Protocol.Screen_Protocol = HID_REPORT_PROTOCOL;
			this -> _Idle.Screen_Idle = 1;
			this -> _epType.Screen_Endpoint_In = USB_ENDPOINT_TYPE_INTERRUPT | USB_ENDPOINT_IN ( 0 );
			this -> _epType.Screen_Endpoint_Out = USB_ENDPOINT_TYPE_INTERRUPT | USB_ENDPOINT_OUT ( 0 );
		#endif

		this -> outputReport_.Length = 0;
		memset ( &this -> featureReport_, 0, sizeof ( this -> featureReport_ ) );

		PluggableUSB ().plug ( this );						// this fait référence au constructeur ( PluggableUSBModule )
	}

	int CLASS_CUSTOM_HID_::getInterface ( uint8_t* _interfaceCount_ ) {
		*_interfaceCount_ += DEVICE_HID_INTERFACE_COUNT;
		HID_Descriptor_t hid_Interface_ = {
			// Device 1 - Les capteurs.
			#if ENABLED(SENSORS_DEVICE)
				D_INTERFACE_ ( SENSORS_HID_INTERFACE_NUMBER, SENSORS_HID_ENDPOINT_COUNT, USB_DEVICE_CLASS_HUMAN_INTERFACE, HID_SUBCLASS_NONE, HID_PROTOCOL_NONE, STRING_IHID_SENSORS_ID ),
				D_HIDREPORT_ ( this -> _DescSize.Sensors_DescriptorSize ),
				D_ENDPOINT   ( USB_ENDPOINT_IN ( SENSORS_HID_ENDPOINT_NUMBER ), USB_ENDPOINT_TYPE_INTERRUPT, EP0_SIZE, 2 ),
			#endif

			// Device 2 - Le débimètre AquaComputer High-Flow 2.
			#if ENABLED(HIGHFLOW2_DEVICE)
				D_INTERFACE_ ( HIGHFLOW2_HID_INTERFACE_NUMBER, HIGHFLOW2_HID_ENDPOINT_COUNT, USB_DEVICE_CLASS_HUMAN_INTERFACE, HID_SUBCLASS_NONE, HID_PROTOCOL_NONE, STRING_IHID_HIGHFLOW2_ID ),
				D_HIDREPORT_ ( this -> _DescSize.HighFlow2_DescriptorSize ),
				D_ENDPOINT   ( USB_ENDPOINT_IN ( HIGHFLOW2_HID_ENDPOINT_NUMBER ), USB_ENDPOINT_TYPE_INTERRUPT, EP0_SIZE, 5 ),
			#endif

			// Device 3 - L'afficheur.
			#if ENABLED(SCREEN_DEVICE)
				D_INTERFACE_ ( SCREEN_HID_INTERFACE_NUMBER, SCREEN_HID_ENDPOINT_COUNT, USB_DEVICE_CLASS_HUMAN_INTERFACE, HID_SUBCLASS_NONE, HID_PROTOCOL_NONE, STRING_IHID_SCREEN_ID ),
				D_HIDREPORT_ ( this -> _DescSize.Screen_DescriptorSize ),
				D_ENDPOINT   ( USB_ENDPOINT_IN ( SCREEN_HID_IN_ENDPOINT_NUMBER ), USB_ENDPOINT_TYPE_INTERRUPT, EP0_SIZE, 8 ),
				D_ENDPOINT   ( USB_ENDPOINT_OUT ( SCREEN_HID_OUT_ENDPOINT_NUMBER ), USB_ENDPOINT_TYPE_INTERRUPT, EP0_SIZE, 8 )
			#endif
		};

		return USBDevice.sendControl ( &hid_Interface_, sizeof ( hid_Interface_ ) );
	}

	/* globalNode_ -> *next_ = device2Node_
	 * globalNode_ -> *data = _Device1_HIDReportDescriptor
	 * globalNode_ -> length = sizeof ( _Device1_HIDReportDescriptor )
	 * globalNode_ -> device = 0
	 * globalNode_ -> interface = 2
	 *
	 * device2Node_ -> *next_ = device3Node_
	 * device2Node_ -> *data = _Device2_HIDReportDescriptor
	 * device2Node_ -> length = sizeof ( _Device2_HIDReportDescriptor )
	 * device2Node_ -> device = 1
	 * device2Node_ -> interface = 3
	 *
	 * device3Node_ -> *next_ = NULL
	 * device3Node_ -> *data = _Device3_HIDReportDescriptor
	 * device3Node_ -> length = sizeof ( _Device3_HIDReportDescriptor )
	 * device3Node_ -> device = 2
	 * device3Node_ -> interface = 4
	 */
	int CLASS_CUSTOM_HID_::getDescriptor ( USBSetup& setup_ ) {
		if ( setup_.bmRequestType != REQUEST_DEVICETOHOST_STANDARD_INTERFACE ) return 0;
		if ( setup_.wValueH != HID_REPORT_DESCRIPTOR_TYPE ) return 0;
		if ( this -> allowed_Interface ( setup_.wIndex ) == false ) return 0;

		int _total = 0;
		HIDSubDescriptor* hidNode_;

		for ( hidNode_ = this -> globalNode_; hidNode_; hidNode_ = hidNode_ -> next_ ) {
			#if (DEVICE_HID_INTERFACE_COUNT == 1)
				int _res = USBDevice.sendControl ( hidNode_ -> data, hidNode_ -> length );
				if ( _res == -1 ) return -1;
				_total += _res;
			#else
				if ( ( hidNode_ -> interface () ) == setup_.wIndex ) return USBDevice.sendControl ( hidNode_ -> data, hidNode_ -> length );
			#endif
		}

		return _total;
	}

	bool CLASS_CUSTOM_HID_::setup ( USBSetup& setup_ ) {
		if ( this -> allowed_Interface ( setup_.wIndex ) == false ) return false;
		uint8_t _tmp;

		switch ( setup_.bmRequestType ) {
			case REQUEST_HOSTTODEVICE_CLASS_INTERFACE:						// bmRequestType = 0x21
				switch ( setup_.bRequest ) {
					case HID_SET_REPORT:							// bRequest = 0x09
						this -> setReport_.Interface = setup_.wIndex;
						this -> setReport_.Type = setup_.wValueH;
						this -> setReport_.ID = setup_.wValueL;
						this -> setReport_.Length = setup_.wLength > EP0_SIZE ? EP0_SIZE : setup_.wLength;

						this -> setReport_.Ready = false;
						USBDevice.recvControl ( this -> setReport_.Report, this -> setReport_.Length );
						this -> setReport_.Ready = true;

						return false;
						break;
					case HID_SET_IDLE:							// bRequest = 0x0A
						this -> set_Idle ( setup_.wIndex, setup_.wValueL );
						return true;
						break;
					case HID_SET_PROTOCOL:							// bRequest = 0x0B
						this -> set_Protocol ( setup_.wIndex, setup_.wValueL );
						return true;
						break;
				}
				break;
			case REQUEST_DEVICETOHOST_CLASS_INTERFACE:						// bmRequestType = 0xA1
				switch ( setup_.bRequest ) {
					case HID_GET_REPORT:							// bRequest = 0x01
						this -> sendCtrlReport ( setup_ );
						return true;
						break;
					case HID_GET_IDLE:							// bRequest = 0x02
						_tmp = this -> get_TheIdle ( setup_.wIndex );
						USBDevice.armSend ( EP0, &_tmp, sizeof ( _tmp ) );
						return true;
						break;
					case HID_GET_PROTOCOL:							// bRequest = 0x03
						_tmp = this -> get_TheProtocol ( setup_.wIndex );
						USBDevice.armSend ( EP0, &_tmp, sizeof ( _tmp ) );
						return true;
						break;
				}
				break;
		}

		return false;
	}

	uint8_t CLASS_CUSTOM_HID_::getShortName ( char* _name_ ) {
		_name_ [ 0 ]  = 'C';
		_name_ [ 1 ]  = 'A';
		_name_ [ 2 ]  = 'N';
		_name_ [ 3 ]  = ' ';
		_name_ [ 4 ]  = 's';
		_name_ [ 5 ]  = 'c';
		_name_ [ 6 ]  = 'r';
		_name_ [ 7 ]  = 'e';
		_name_ [ 8 ]  = 'e';
		_name_ [ 9 ]  = 'n';
		_name_ [ 10 ] = ' ';
		_name_ [ 11 ] = 'v';
		_name_ [ 12 ] = '1';

		return 13;
	}

	int CLASS_CUSTOM_HID_::RecvControl ( uint8_t *_data_, uint16_t _dataLength_, uint32_t _length_ ) {
		uint8_t _size = EP0_SIZE;
		uint8_t _dataSize = EP0_SIZE;
		uint16_t _offset = 0;
		const uint8_t* _get = ( const uint8_t* ) _data_;

		while ( _length_ ) {
			if ( _size > _length_ ) _size = _length_;
			_length_ -= _size;
			if ( _dataSize > ( _dataLength_ - _offset ) ) _dataSize = _dataLength_ - _offset;

			USBDevice.recvControl ( ( uint8_t* ) _get, _size );
			if ( _offset < _dataLength_ ) {
				memcpy ( &_data_ [ _offset ], _get, _dataSize );
				_offset += _dataSize;
			}
			_get += _size;
		}

		return _dataSize;
	}

	/* globalNode_ -> *next_ = device2Node_
	 * globalNode_ -> *data = _Device1_HIDReportDescriptor
	 * globalNode_ -> length = sizeof ( _Device1_HIDReportDescriptor )
	 * globalNode_ -> device = 0
	 * globalNode_ -> interface = 2
	 *
	 * device2Node_ -> *next_ = device3Node_
	 * device2Node_ -> *data = _Device2_HIDReportDescriptor
	 * device2Node_ -> length = sizeof ( _Device2_HIDReportDescriptor )
	 * device2Node_ -> device = 1
	 * device2Node_ -> interface = 3
	 *
	 * device3Node_ -> *next_ = NULL
	 * device3Node_ -> *data = _Device3_HIDReportDescriptor
	 * device3Node_ -> length = sizeof ( _Device3_HIDReportDescriptor )
	 * device3Node_ -> device = 2
	 * device3Node_ -> interface = 4
	 */
	void CLASS_CUSTOM_HID_::AppendDescriptor ( HIDSubDescriptor* node_ ) {
		if ( !this -> globalNode_ ) this -> globalNode_ = node_;				// Si globalNode_ = NULL alors globalNode_ = node_
		else {											// Sinon,
			HIDSubDescriptor *current_ = this -> globalNode_;				//	current_ = globalNode_
			while ( current_ -> next_ ) { current_ = current_ -> next_; }			//	Tant que next_ n'est pas NULL, alors current_ = next_
			current_ -> next_ = node_;							//	next_ ( NULL ) = node_
		}

		this -> set_DescriptorSize ( node_ -> device, node_ -> length );
	}

	void CLASS_CUSTOM_HID_::AppendFeature ( uint8_t _featureNB_, void* _featurePtr_, uint32_t _featureLength_, uint8_t _featureID_ ) {
		HID_Feature_t _feature;

		_feature.Report_Ptr = ( uint8_t* ) _featurePtr_;
		_feature.Length = _featureLength_;
		_feature.ID = _featureID_;
		memcpy ( &this -> featureReport_ [ _featureNB_ ], &_feature, sizeof ( _feature ) );
	}

	int CLASS_CUSTOM_HID_::sendCtrlReport ( USBSetup& setup_ ) {
		if ( this -> is_Plugged () == false ) return -1;
		//if ( this -> get_TheIdle ( setup_.wIndex ) > 0 ) return -1;

		if ( setup_.wValueH == HID_FEATURE_REPORT_TYPE ) {
			uint8_t _index;
			switch ( setup_.wValueL ) {
				case 0:
					_index = no_feature_ID;
					break;
				case TemperatureFeature_ID:
				case Sensors_TemperatureReport_ID:
					_index = feature_SensorsTemperature;
					break;
				case MonitorFeature_ID:
				case Sensors_MonitorReport_ID:
					_index = feature_SensorsMonitor;
					break;
				case HighFlow2_TemperatureReport_ID:
					_index = feature_HighFlow2Temperature;
					break;
				case HighFlow2_FlowReport_ID:
					_index = feature_HighFlow2Flow;
					break;
				case Firmware_ID:
					_index = feature_Firmware;
					break;
				case Clock_ID:
					_index = feature_Clock;
					break;
				case DayOffset_ID:
					_index = feature_DayOffset;
					break;
				case Screen_ID:
					_index = feature_Screen;
					break;
				case Dialog_ID:
					_index = feature_Dialog;
					break;
				default:
					return -1;
			}

			static uint8_t _data [ EPX_SIZE ];
			memset ( _data, 0, sizeof ( _data ) );
			//uint32_t _length = min ( this -> featureReport_ [ _index ].Length, setup_.wLength );
			uint32_t _length;
			if ( _index == no_feature_ID ) {				// Cas des FEATURES sans ID !
				_length = min ( this -> featureReport_ [ _index ].Length, ( uint32_t ) setup_.wLength );
				if ( _length > EPX_SIZE ) return -1;
				memcpy ( _data, this -> featureReport_ [ 0 ].Report_Ptr, _length );
			}
			else {								// Cas des FEATURES avec ID !
				_length = min ( this -> featureReport_ [ _index ].Length, ( uint32_t ) ( setup_.wLength - 1 ) );
				if ( _length > ( EPX_SIZE - 1 ) ) return -1;
				_data [ 0 ] = this -> featureReport_ [ _index ].ID;
				memcpy ( &_data [ 1 ], this -> featureReport_ [ _index ].Report_Ptr, _length );
				_length += 1;
			}

			USBDevice.sendControl ( _data, _length );
			return _length;
			//USBDevice.sendControl ( _data, setup_.wLength );
			//return setup_.wLength;
		}

		return -1;
	}

	int CLASS_CUSTOM_HID_::sendReport ( const void* _data_, uint32_t _length_, uint8_t _device_ ) {
		if ( this -> is_Plugged () == false ) return -1;
		//if ( this -> idle ( _device_ ) ) return -1;
		//if ( _length_ > EPX_SIZE ) return -1;

		//uint8_t _data [ EPX_SIZE ];
		//memcpy ( _data, _data_, _length_ );
		const uint8_t* _data = reinterpret_cast < const uint8_t* > ( _data_ );

		uint8_t _size = EP0_SIZE;
		const uint8_t* _send = ( const uint8_t* ) _data;
		while ( _length_ ) {
			if ( _size > _length_ ) _size = _length_;
			_length_ -= _size;

			USBDevice.send ( CDC_ENPOINT_COUNT + _device_ + 1, _send, _size );				// CDC_ENPOINT_COUNT + _device_ + 1 retourne le numéro de endpoint
			_send += _size;
		}

		return _length_;
	}

	int CLASS_CUSTOM_HID_::sendReport ( uint8_t _reportID_, const void* _data_, int _length_, uint8_t _device_ ) {
		if ( this -> is_Plugged () == false ) return -1;
		//if ( this -> idle ( _device_ ) ) return -1;
		if ( _length_ > ( EPX_SIZE - 1 ) ) return -1;

		uint8_t _data [ EPX_SIZE ];
		_data [ 0 ] = _reportID_;
		memcpy ( &_data [ 1 ], _data_, _length_ );
		_length_ += 1;

		uint8_t _size = EP0_SIZE;
		const uint8_t* _send = ( const uint8_t* ) _data;
		while ( _length_ ) {
			if ( _size > _length_ ) _size = _length_;
			_length_ -= _size;

			USBDevice.send ( CDC_ENPOINT_COUNT + _device_ + 1, _send, _size );
			_send += _size;
		}

		return _length_;
	}

	uint8_t CLASS_CUSTOM_HID_::getReport ( uint8_t* _reportType_, uint8_t* _interface_, void* _buffer_ ) {
		if ( ( this -> setReport_.Ready ) && ( this -> setReport_.Length > 2 ) ) {
			memcpy ( _buffer_, this -> setReport_.Report, this -> setReport_.Length );
			*_reportType_ = ( uint8_t ) this -> setReport_.Type;
			*_interface_ = ( uint8_t ) this -> setReport_.Interface;

			this -> setReport_.Ready = false;
			return this -> setReport_.Length;
		}
		else return 0;
	}

	uint8_t CLASS_CUSTOM_HID_::getReport ( uint8_t* _reportType_, uint8_t* _reportID_, uint8_t* _interface_, void* _buffer_ ) {
		if ( ( this -> setReport_.Ready ) && ( this -> setReport_.Length > 2 ) ) {
			//memcpy ( _buffer_, this -> setReport_.Report, this -> setReport_.Length );
			this -> setReport_.Length --;
			memcpy ( _buffer_, &this -> setReport_.Report [ 1 ], this -> setReport_.Length );
			*_reportType_ = ( uint8_t ) this -> setReport_.Type;
			*_reportID_ = ( uint8_t ) this -> setReport_.ID;
			*_interface_ = ( uint8_t ) this -> setReport_.Interface;

			this -> setReport_.Ready = false;
			return this -> setReport_.Length;
		}
		else return 0;
	}

	uint8_t CLASS_CUSTOM_HID_::getOutput ( void* _buffer_ ) {
		if ( this -> outputReport_.Length > 2 ) {
			memset ( _buffer_, 0, EP0_SIZE - 1 );								// Initialisation du buffer à 0.
			memcpy ( _buffer_, &this -> outputReport_.Report [ 1 ], this -> outputReport_.Length - 1 );	// Écriture des données reçues dans _buffer.
			return ( this -> outputReport_.Length - 1 );							// On donne la taille du report - le byte contenant l'ID ( REPORT_ID ).
		}
		else return 0;
	}
#endif



/*
 * 		Ré-écriture de la routine d'interruption USB fournie par Arduino
 *
 */
//#include "USB/USBCore.cpp"
/*#ifdef __SAMR21G18A__
	extern USBDevice_SAMR21G18x usbd;
#else
	extern USBDevice_SAMD21G18x usbd;
#endif*/

//extern char isRemoteWakeUpEnabled;
//extern char isEndpointHalt;
//extern volatile uint32_t _usbConfiguration;
//extern volatile uint32_t _usbSetInterface;
//extern uint8_t udd_ep_out_cache_buffer [ 7 ] [ 64 ];
//extern EPHandler *epHandlers[7];
//extern bool _pack_message;

//const DeviceDescriptor USB_DeviceDescriptorC = D_DEVICE ( 0xEF, 0x02, 0x01, 64, 0x2341, 0x003E, 0x100, IMANUFACTURER, IPRODUCT, ISERIAL, 1 );

/*bool sendStringDescriptor2 ( const uint8_t *_string_, uint32_t _maxlen_ ) {
	if ( _maxlen_ < 2 ) return false;

	uint8_t* _buffer = ( uint8_t* ) malloc ( _maxlen_ );
	_buffer [ 0 ] = ( strlen ( ( const char* ) _string_ ) * 2 ) + 2;		// bLength
	_buffer [ 1 ] = 0x03;								// bDescriptorType	STRING

	uint8_t _pos;
	for ( _pos = 2; _pos < ( _maxlen_ && *_string_ ); _pos ++ ) {
		_buffer [ _pos ++ ] = *_string_ ++;
		if ( _pos == _maxlen_ ) break;
		_buffer [ _pos ] = 0;
	}

	uint32_t _return = USBDevice.sendControl ( _buffer, _pos );			// Limité à 256 caractères, mais retourne toujours _pos !
	free ( _buffer );
	return ( _return > 0 ? true : false );
}*/

/*bool sendDescriptor2 ( USBSetup& setup_ ) {
	uint8_t _type = setup_.wValueH;
	bool _cdcComposite;
	uint8_t _desc_Length = 0;
	const uint8_t *_desc_Addr = 0;

	if ( _type == USB_CONFIGURATION_DESCRIPTOR_TYPE ) return USBDevice.sendConfiguration ( setup_.wLength );

	#ifdef PLUGGABLE_USB_ENABLED
		int _return = PluggableUSB ().getDescriptor ( setup_ );
		if ( _return != 0 ) return ( _return > 0 ? true : false );
	#endif

	if ( _type == USB_DEVICE_DESCRIPTOR_TYPE ) {
		if ( setup_.wLength == 8 ) _desc_Addr = ( const uint8_t* ) &USB_DeviceDescriptorC;
		else _desc_Addr = ( const uint8_t* ) &USB_DeviceDescriptor;

		if ( *_desc_Addr > setup_.wLength ) _desc_Length = setup_.wLength;
	}
	else if ( _type == USB_STRING_DESCRIPTOR_TYPE ) {
		if ( setup_.wValueL == 0 )				_desc_Addr = ( const uint8_t* ) &STRING_LANGUAGE;
		else if ( setup_.wValueL == IPRODUCT )			return sendStringDescriptor2 ( STRING_PRODUCT,			setup_.wLength );
		else if ( setup_.wValueL == IMANUFACTURER )		return sendStringDescriptor2 ( STRING_MANUFACTURER,		setup_.wLength );

		else if ( setup_.wValueL == STRING_IHID_NAME_ID )	return sendStringDescriptor2 ( STRING_IHID_NAME_STRING,	setup_.wLength );
		else if ( setup_.wValueL == STRING_IHID_UNUSED_ID )	return sendStringDescriptor2 ( STRING_IHID_UNUSED_STRING,	setup_.wLength );
		else if ( setup_.wValueL == STRING_IHID_FLOW_ID )	return sendStringDescriptor2 ( STRING_IHID_FLOW_STRING,	setup_.wLength );

		else if ( setup_.wValueL == ISERIAL )			return sendStringDescriptor2 ( STRING_SERIAL,			setup_.wLength );
		//else if ( setup_.wValueL == ISERIAL ) {
		//	#define SERIAL_NUMBER_WORD_0	*(volatile uint32_t*)(0x008061FC)
		//	#define SERIAL_NUMBER_WORD_1	*(volatile uint32_t*)(0x00806010)
		//	#define SERIAL_NUMBER_WORD_2	*(volatile uint32_t*)(0x00806014)
		//	#define SERIAL_NUMBER_WORD_3	*(volatile uint32_t*)(0x00806018)

		//	char _name [ ISERIAL_MAX_LEN ];
		//	utox8 ( SERIAL_NUMBER_WORD_0, &_name [ 0 ] );
		//	utox8 ( SERIAL_NUMBER_WORD_1, &_name [ 8 ] );
		//	utox8 ( SERIAL_NUMBER_WORD_2, &_name [ 16 ] );
		//	utox8 ( SERIAL_NUMBER_WORD_3, &_name [ 24 ] );
		//	_name [ 32 ] = '\0';
		//	return sendStringDescriptor2 ( ( uint8_t* ) _name, setup_.wLength );
		//}
		else return false;
		if ( *_desc_Addr > setup_.wLength ) _desc_Length = setup_.wLength;
	}
	else {}

	if ( _desc_Addr == 0 ) return false;
	if ( _desc_Length == 0 ) _desc_Length = *_desc_Addr;

	USBDevice.sendControl ( _desc_Addr, _desc_Length );
	return true;
}*/

/*void USB_0_Handler () {
	if ( _pack_message ) return;

	if ( usbd.isEndOfResetInterrupt () ) {
		USBDevice.initEP ( EP0, USB_ENDPOINT_TYPE_CONTROL );
		usbd.epBank0EnableSetupReceived ( EP0 );

		_usbConfiguration = 0;
		usbd.ackEndOfResetInterrupt ();
	}

	if ( usbd.isStartOfFrameInterrupt () ) usbd.ackStartOfFrameInterrupt ();

	// Endpoint 0 Received Setup interrupt
	if ( usbd.epBank0IsSetupReceived ( EP0 ) ) {
		usbd.epBank0AckSetupReceived ( EP0 );

		USBSetup *setup_ = reinterpret_cast < USBSetup * > ( udd_ep_out_cache_buffer [ 0 ] );
		usbd.epBank0ResetReady ( EP0 );

		uint8_t _requestType = setup_ -> bmRequestType;
		bool _ok = true;
		if ( ( _requestType & REQUEST_TYPE ) == REQUEST_STANDARD ) {					// Si ( ( bmRequestType & 0x60 ) == 0x00 )
			// Standard Requests
			switch ( setup_ -> bRequest ) {
				case GET_STATUS:								// bRequest = 0x00
					if ( setup_ -> bmRequestType == 0 ) {
						uint8_t _buffer [ 2 ] = { 0, 0 };
						USBDevice.armSend ( EP0, _buffer, sizeof ( _buffer ) );
					}
					else {
						uint8_t _buffer [ 2 ] = { 0, 0 };
						if ( isEndpointHalt == 1 ) _buffer [ 0 ] = 1;
						USBDevice.armSend ( EP0, _buffer, sizeof ( _buffer ) );
					}
					break;
				case CLEAR_FEATURE:								// bRequest = 0x01
					if ( setup_ -> wValueL == 1 ) {
						uint8_t _buffer [ 2 ] = { 0, 0 };
						if ( isRemoteWakeUpEnabled == 1 ) _buffer [ 0 ] = 1;
						USBDevice.armSend ( EP0, _buffer, sizeof ( _buffer ) );
					}
					else {
						isEndpointHalt = 0;
						USBDevice.sendZlp ( EP0 );
					}
					break;
				case SET_FEATURE:								// bRequest = 0x03
					if ( setup_ -> wValueL == 1 ) {
						uint8_t _buffer [ 1 ] = { 0 };
						isRemoteWakeUpEnabled = 1;
						USBDevice.armSend ( EP0, _buffer, sizeof ( _buffer ) );
					}
					if ( setup_ -> wValueL == 0 ) {
						isEndpointHalt = 1;
						USBDevice.sendZlp ( EP0 );
					}
					_ok = false;
					break;
				case SET_ADDRESS:								// bRequest = 0x05
					USBDevice.setAddress ( setup_ -> wValueL );
					break;
				case GET_DESCRIPTOR:								// bRequest = 0x06
					//_ok = USBDevice.sendDescriptor ( *setup_ );
					_ok = sendDescriptor2 ( *setup_ );
					break;
				case SET_DESCRIPTOR:								// bRequest = 0x07
					_ok = false;
					break;
				case GET_CONFIGURATION:								// bRequest = 0x08
					USBDevice.armSend ( EP0, ( void* ) &_usbConfiguration, sizeof ( _usbConfiguration ) );
					break;
				case SET_CONFIGURATION:								// bRequest = 0x09
					if ( ( _requestType & REQUEST_RECIPIENT ) == REQUEST_DEVICE ) {
						USBDevice.initEndpoints ();
						_usbConfiguration = setup_ -> wValueL;

						#ifdef CDC_ENABLED
							usbd.epBank1EnableTransferComplete ( CDC_ENDPOINT_ACM );
							usbd.epBank0EnableTransferComplete ( CDC_ENDPOINT_OUT );
						#endif

						USBDevice.sendZlp ( EP0 );
					}
					else _ok = false;
					break;
				case GET_INTERFACE:								// bRequest = 0x0A
					USBDevice.armSend ( EP0, ( void* ) _usbSetInterface, sizeof ( _usbSetInterface ) );
					break;
				case SET_INTERFACE:								// bRequest = 0x0B
					_usbSetInterface = setup_ -> wValueL;
					USBDevice.sendZlp ( EP0 );
					break;
			}
		}
		else {
			#if defined(CDC_ENABLED)
				if ( setup_ -> wIndex == CDC_ACM_INTERFACE ) {
					if ( CDC_Setup ( *setup_ ) == false ) USBDevice.sendZlp ( EP0 );
				}
			#endif
			#if defined(PLUGGABLE_USB_ENABLED)
				bool _return = PluggableUSB ().setup ( *setup_ );
				if ( _return == false ) USBDevice.sendZlp ( EP0 );
				_ok = _return;
			#endif
			_ok = false;
		}

		if ( _ok ) usbd.epBank1SetReady ( EP0 );
		else USBDevice.stall ( EP0 );

		if ( usbd.epBank1IsStalled ( EP0 ) ) {
			usbd.epBank1AckStalled ( EP0 );
			usbd.epBank1DisableStalled ( EP0 );
		}
	}

	uint8_t _interrupt = 0;
	uint8_t _endpoint_interrupt = usbd.epInterruptSummary () & 0xFE;
	while ( _endpoint_interrupt != 0 ) {
		if ( ( _endpoint_interrupt & ( 1 << _interrupt ) ) != 0 ) {
			if ( usbd.epBank0IsTransferComplete ( _interrupt ) || usbd.epBank1IsTransferComplete (_interrupt ) ) {
				if ( epHandlers [ _interrupt ] ) epHandlers [ _interrupt ] -> handleEndpoint ();
				else USBDevice.handleEndpoint ( _interrupt );
			}
			_endpoint_interrupt &= ~( 1 << _interrupt );
		}
		_interrupt ++;
		if ( _interrupt > USB_EPT_NUM ) break;
	}
}*/
