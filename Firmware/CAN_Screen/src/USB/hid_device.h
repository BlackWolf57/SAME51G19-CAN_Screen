#pragma once

#include <stdint.h>
#include "USB/PluggableUSB.h"
#include "USB/USBDesc.h"
#include "../board/enums.h"
#include "../board/macros.h"

#if defined(USBCON)
	#define EP0_SIZE					64
	#define SENSORS_DEVICE
	#define HIGHFLOW2_DEVICE
	#define SCREEN_DEVICE

	#if ENABLED(SENSORS_DEVICE)																// CDC		Sensors Device			High-Flow 2 Device		Screen Device
		#define SENSORS_HID_INTERFACE_COUNT		1												// Non		Interface n° 0, Endpoint n° 1 (In)
		#define SENSORS_HID_INTERFACE_NUMBER		CDC_INTERFACE_COUNT		// = 2								// Oui		Interface n° 2, Endpoint n° 4 (In)
		#define SENSORS_HID_ENDPOINT_COUNT		1
		#define SENSORS_HID_ENDPOINT_NUMBER		(CDC_ENPOINT_COUNT + 1)		// = 3 + 1
	#else
		#define SENSORS_HID_INTERFACE_COUNT		0
		#define SENSORS_HID_INTERFACE_NUMBER		0
		#define SENSORS_HID_ENDPOINT_COUNT		0
		#define SENSORS_HID_ENDPOINT_NUMBER		0
	#endif
	#define SENSORS_HID_ENDPOINT_POSITION			0

	#if ENABLED(HIGHFLOW2_DEVICE)
		#define HIGHFLOW2_HID_INTERFACE_COUNT		1												// Non			Non			Interface n° 0, Endpoint n° 1 (In)
		#define HIGHFLOW2_HID_INTERFACE_NUMBER		(CDC_INTERFACE_COUNT + SENSORS_HID_INTERFACE_COUNT)						// Non			Oui			Interface n° 1, Endpoint n° 2 (In)
		#define HIGHFLOW2_HID_ENDPOINT_COUNT		1												// Oui			Non			Interface n° 2, Endpoint n° 4 (In)
		#define HIGHFLOW2_HID_ENDPOINT_NUMBER		(CDC_ENPOINT_COUNT + SENSORS_HID_ENDPOINT_COUNT + 1)						// Oui			Oui			Interface n° 3, Endpoint n° 5 (In)
	#else
		#define HIGHFLOW2_HID_INTERFACE_COUNT		0
		#define HIGHFLOW2_HID_INTERFACE_NUMBER		0
		#define HIGHFLOW2_HID_ENDPOINT_COUNT		0
		#define HIGHFLOW2_HID_ENDPOINT_NUMBER		0
	#endif
	#define HIGHFLOW2_HID_ENDPOINT_POSITION			(SENSORS_HID_ENDPOINT_POSITION + SENSORS_HID_ENDPOINT_COUNT)

	#if ENABLED(SCREEN_DEVICE)
		#define SCREEN_HID_INTERFACE_COUNT		1												// Non			Non				Non			Interface n° 0, Endpoints n° 1 (In), 2 (Out)
		#define SCREEN_HID_INTERFACE_NUMBER		(CDC_INTERFACE_COUNT + SENSORS_HID_INTERFACE_COUNT + HIGHFLOW2_HID_INTERFACE_COUNT)		// Non			Non				Oui			Interface n° 1, Endpoints n° 2 (In), 3 (Out)
		#define SCREEN_HID_ENDPOINT_COUNT		2												// Non			Oui				Non			Interface n° 1, Endpoints n° 2 (In), 3 (Out)
		#define SCREEN_HID_IN_ENDPOINT_NUMBER		(CDC_ENPOINT_COUNT + SENSORS_HID_ENDPOINT_COUNT + HIGHFLOW2_HID_ENDPOINT_COUNT + 1)		// Non			Oui				Oui			Interface n° 2, Endpoints n° 3 (In), 4 (Out)
		#define SCREEN_HID_OUT_ENDPOINT_NUMBER		(SCREEN_HID_IN_ENDPOINT_NUMBER + 1)								// Oui			Non				Non			Interface n° 2, Endpoints n° 4 (In), 5 (Out)
	#else																			// Oui			Non				Oui			Interface n° 3, Endpoints n° 5 (In), 6 (Out)
		#define SCREEN_HID_INTERFACE_COUNT		0												// Oui			Oui				Non			Interface n° 3, Endpoints n° 5 (In), 6 (Out)
		#define SCREEN_HID_INTERFACE_NUMBER		0												// Oui			Oui				Oui			Interface n° 4, Endpoints n° 6 (In), 7 (Out)
		#define SCREEN_HID_ENDPOINT_COUNT		0
		#define SCREEN_HID_IN_ENDPOINT_NUMBER		0
		#define SCREEN_HID_OUT_ENDPOINT_NUMBER		0
	#endif
	#define SCREEN_HID_ENDPOINT_POSITION			(HIGHFLOW2_HID_ENDPOINT_POSITION + HIGHFLOW2_HID_ENDPOINT_COUNT)

	#define DEVICE_HID_INTERFACE_COUNT			(SENSORS_HID_INTERFACE_COUNT + HIGHFLOW2_HID_INTERFACE_COUNT + SCREEN_HID_INTERFACE_COUNT)	// Normalement 3.	De 1 à 3 suivant les options de compilation
	#define DEVICE_HID_ENDPOINT_COUNT			(SENSORS_HID_ENDPOINT_COUNT + HIGHFLOW2_HID_ENDPOINT_COUNT + SCREEN_HID_ENDPOINT_COUNT)		// Normalement 4.	De 1 à 4 suivant les options de compilation

	#define HID_INPUT_REPORT_TYPE				1
	#define HID_OUTPUT_REPORT_TYPE				2
	#define HID_FEATURE_REPORT_TYPE				3

	#define HID_GET_REPORT					0x01
	#define HID_GET_IDLE					0x02
	#define HID_GET_PROTOCOL				0x03
	#define HID_SET_REPORT					0x09
	#define HID_SET_IDLE					0x0A
	#define HID_SET_PROTOCOL				0x0B

	#define HID_HID_DESCRIPTOR_TYPE				0x21
	#define HID_REPORT_DESCRIPTOR_TYPE			0x22
	#define HID_PHYSICAL_DESCRIPTOR_TYPE			0x23

	// HID subclass HID 1.11 Page 8 - 4.2 Subclass
	#define HID_SUBCLASS_NONE				0
	#define HID_SUBCLASS_BOOT_INTERFACE			1

	// HID Keyboard/Mouse bios compatible protocols HID 1.11 Page 9 - 4.3 Protocols
	#define HID_PROTOCOL_NONE				0
	#define HID_PROTOCOL_KEYBOARD				1
	#define HID_PROTOCOL_MOUSE				2

	// Normal or bios protocol (Keyboard/Mouse) HID 1.11 Page 54 - 7.2.5 Get_Protocol Request
	// "protocol" variable is used for this purpose
	#define HID_BOOT_PROTOCOL				0
	#define HID_REPORT_PROTOCOL				1

	/** Informations sur le Idle:
	 *  wValue & 0x00FF contient l'éventuel ID. Si 0 => Idle s'applique à tous les reports, si ID => Idle s'applique au report ID.
	 *  wValue & 0xFF00 contient la durée ( valeur x 4 millisecondes ). Si valeur = 0 alors attente infinie et aucun report. Sinon, Idle de # en boucle infinie
	 */

	typedef struct {
		uint8_t len;							// 9		( Fixé en dur )
		uint8_t dtype;							// 0x21		( HID_HID_DESCRIPTOR_TYPE )
		uint8_t addr;
		uint8_t versionL;						// 0x11		( Fixé en dur		-> Version 1.11 )
		uint8_t versionH;						// 0x01		( Fixé en dur )
		uint8_t country;						// 0		( Fixé en dur		0 -> Not specified )
		uint8_t desctype;						// 0x22		( HID_REPORT_DESCRIPTOR_TYPE )
		uint8_t descLenL;						//		( Calculé )
		uint8_t descLenH;						//		( Calculé )
	} HID_DescDescriptor_t;

	typedef struct {
		// Device 1 - Les capteurs.
		#if ENABLED(SENSORS_DEVICE)
			InterfaceDescriptor				hid_device1_interface;
			HID_DescDescriptor_t				hid_device1_descriptor;
			EndpointDescriptor				hid_device1_IN_endpoint;
		#endif

		// Device 2 - Le débimètre AquaComputer High-Flow 2.
		#if ENABLED(HIGHFLOW2_DEVICE)
			InterfaceDescriptor				hid_device2_interface;
			HID_DescDescriptor_t				hid_device2_descriptor;
			EndpointDescriptor				hid_device2_IN_endpoint;
		#endif

		// Device 3 - L'afficheur.
		#if ENABLED(SCREEN_DEVICE)
			InterfaceDescriptor				hid_device3_interface;
			HID_DescDescriptor_t				hid_device3_descriptor;
			EndpointDescriptor				hid_device3_IN_endpoint;
			EndpointDescriptor				hid_device3_OUT_endpoint;
		#endif
	} HID_Descriptor_t;

	typedef struct {
		volatile bool Ready;
		uint8_t Interface;
		uint8_t Type;
		uint8_t ID;
		uint8_t Report [ EP0_SIZE ];
		uint16_t Length;
	} HID_setReport_t;										// Entrées pour les requêtes SET_REPORT

	typedef struct {										// Entrées pour les requêtes IN_REPORT
		uint8_t Report [ EP0_SIZE ];
		uint16_t Length;
	} HID_Report_t;

	typedef struct {
		uint8_t* Report_Ptr;									// Pointeur mémoire vers la feature.
		uint32_t Length;									// Nombre de bytes dans la feature.
		uint8_t ID;
	} HID_Feature_t;

	class HIDSubDescriptor {
	public:
		HIDSubDescriptor *next_ = NULL;
		HIDSubDescriptor ( const void *_data_, const uint16_t _length_, const uint8_t _device_ ) : data ( _data_ ),
													   length ( _length_ ),
													   device ( _device_ )
		{ /* Empty */ }

		const void* data;									// Les données à transmettre par le ENDPOINT.
		const uint16_t length;									// La taille de ces données.
		const uint8_t device;									// Numéro d'interface interne. ( Entre 0 et 2 )
		const uint8_t interface ()	{ return this -> device + CDC_INTERFACE_COUNT; }	// Numéro d'interface HID. ( Entre 2 et 4 si CDC )
	};

	class CLASS_CUSTOM_HID_ : public PluggableUSBModule {
	protected:
		int getInterface ( uint8_t* _interfaceCount_ );
		int getDescriptor ( USBSetup& setup_ );
		bool setup ( USBSetup& setup_ );
		uint8_t getShortName ( char* _name_ );
		int RecvControl ( uint8_t *_data_, uint16_t _dataLength_, uint32_t _length_ );

	private:
		HIDSubDescriptor* globalNode_;
		union {
			struct {
				#if ENABLED(SENSORS_DEVICE)
					uint8_t Sensors_Idle;				// 125 -> 0x7D
				#endif
				#if ENABLED(HIGHFLOW2_DEVICE)
					uint8_t HighFlow2_Idle;				// 250 -> 0xFA
				#endif
				#if ENABLED(SCREEN_DEVICE)
					uint8_t Screen_Idle;
				#endif
			};
			uint8_t Idle [ DEVICE_HID_INTERFACE_COUNT ];			// Devrait être 0x7D, 0xFA, mais semble être 0x00, 0x20
		} _Idle;
		union {
			struct {
				#if ENABLED(SENSORS_DEVICE)
					uint8_t Sensors_Protocol;
				#endif
				#if ENABLED(HIGHFLOW2_DEVICE)
					uint8_t HighFlow2_Protocol;
				#endif
				#if ENABLED(SCREEN_DEVICE)
					uint8_t Screen_Protocol;
				#endif
			};
			uint8_t Protocol [ DEVICE_HID_INTERFACE_COUNT ];
		} _Protocol;
		union {
			struct {
				#if ENABLED(SENSORS_DEVICE)
					uint16_t Sensors_DescriptorSize;
				#endif
				#if ENABLED(HIGHFLOW2_DEVICE)
					uint16_t HighFlow2_DescriptorSize;
				#endif
				#if ENABLED(SCREEN_DEVICE)
					uint16_t Screen_DescriptorSize;
				#endif
			};
			uint16_t DescriptorSize [ DEVICE_HID_INTERFACE_COUNT ];
		} _DescSize;
		union {
			struct {
				#if ENABLED(SENSORS_DEVICE)
					uint32_t Sensors_Endpoint_In;
				#endif
				#if ENABLED(HIGHFLOW2_DEVICE)
					uint32_t HighFlow2_Endpoint_In;
				#endif
				#if ENABLED(SCREEN_DEVICE)
					uint32_t Screen_Endpoint_In;
					uint32_t Screen_Endpoint_Out;
				#endif
			};
			uint32_t EndpointType [ DEVICE_HID_ENDPOINT_COUNT ];
		} _epType;

		HID_setReport_t setReport_;
		HID_Report_t outputReport_;
		HID_Feature_t featureReport_ [ feature_Max_Count ];

		//bool allowed_Interface ( uint8_t _interface_ ) const					{ return ( ( _interface_ >= CDC_INTERFACE_COUNT ) && ( _interface_ < ( CDC_INTERFACE_COUNT + DEVICE_HID_INTERFACE_COUNT ) ) ); }
		bool allowed_Interface ( uint8_t _interface_ ) const					{ return true; }
		void set_Protocol ( uint8_t _interface_, uint8_t _value_ )				{ this -> _Protocol.Protocol [ _interface_ - CDC_INTERFACE_COUNT ] = _value_; }
		void set_Idle ( uint8_t _interface_, uint8_t _value_ )					{ this -> _Idle.Idle [ _interface_ - CDC_INTERFACE_COUNT ] = _value_; }
		void set_DescriptorSize ( uint8_t _interface_, uint16_t _value_ )			{ this -> _DescSize.DescriptorSize [ _interface_ ] = _value_; }
		uint8_t get_TheProtocol ( uint8_t _interface_ ) const					{ return this -> _Protocol.Protocol [ _interface_ - CDC_INTERFACE_COUNT ]; }
		uint8_t get_TheIdle ( uint8_t _interface_ ) const					{ return this -> _Idle.Idle [ _interface_ - CDC_INTERFACE_COUNT ]; }

	public:
		CLASS_CUSTOM_HID_ ();
		void AppendDescriptor ( HIDSubDescriptor* node_ );
		void AppendFeature ( uint8_t _featureNB_, void* _featurePtr_, uint32_t _featureLength_, uint8_t _featureID_ );
		void AppendFeature ( void* _featurePtr_, uint32_t _featureLength_ )			{ this -> AppendFeature ( 0, _featurePtr_, _featureLength_, 0 ); }
		int sendCtrlReport ( USBSetup& setup_ );
		int sendReport ( const void* _data_, uint32_t _length_, uint8_t _device_ );
		int sendReport ( uint8_t _reportID_, const void* _data_, int _length_, uint8_t _device_ );
		uint8_t getReport ( uint8_t* _reportType_, uint8_t* _interface_, void* _buffer_ );
		uint8_t getReport ( uint8_t* _reportType_, uint8_t* _reportID_, uint8_t* _interface_, void* _buffer_ );
		uint8_t getOutput ( void* _buffer_ );
		//uint8_t getFeature ( uint8_t* _buffer_ );
		uint16_t requestReport ( uint8_t* _reportID_, uint8_t* _type_ );
		void clearReport ()									{ memset ( &this -> setReport_, 0, sizeof ( this -> setReport_ ) ); }
		void clearOutput ()									{ memset ( &this -> outputReport_, 0, sizeof ( this -> outputReport_ ) ); }
		bool is_Plugged () const								{ return ( USB -> DEVICE.STATUS.bit.LINESTATE > 0 ); }
		bool isReadyToTransmit ( uint32_t _endpoint_ ) const					{ return !USB -> DEVICE.DeviceEndpoint [ _endpoint_ ].EPSTATUS.bit.BK1RDY; }
		//bool can_Write ( uint8_t _device_ ) const						{ return ( ( this -> is_Plugged () ) && ( this -> idle ( _device_ ) == false ) && ( this -> getReport_.wLength == 0 ) ); }
		//bool can_Write ( uint8_t _device_ ) const						{ return ( ( this -> is_Plugged () ) && ( this -> idle ( _device_ ) == false ) ); }
		bool can_Write ( uint8_t _device_ ) const						{ return true; }
		//bool idle ( uint8_t _device_ ) const							{ return ( this -> _Idle.Idle [ _device_ ] > 0 ); }
		//bool idle ( uint8_t _device_ ) const							{ return false; }
		uint8_t get_Protocol ( uint8_t _interface_ ) const					{ return this -> _Protocol.Protocol [ _interface_ ]; }
		uint8_t get_Idle ( uint8_t _interface_ ) const						{ return this -> _Idle.Idle [ _interface_ ]; }
		int begin ()										{ return 0; }
	};
	CLASS_CUSTOM_HID_& customHID_ ();

	#define D_HIDREPORT_(length) {			\
			9,				\
			HID_HID_DESCRIPTOR_TYPE,	\
			0x11,				\
			0x01,				\
			0,				\
			1,				\
			HID_REPORT_DESCRIPTOR_TYPE,	\
			lowByte(length),		\
			highByte(length)		\
	}
	#define D_INTERFACE_(n, bNumEndpoints, bClass, bSubClass, bProtocol, iInterface) { \
			9,				\
			4,				\
			(uint8_t) n,			\
			0,				\
			bNumEndpoints,			\
			bClass,				\
			bSubClass,			\
			bProtocol,			\
			iInterface			\
	}

	/* HID_DESCRIPTOR {
	 * 		bLength = 9
	 * 		bDescriptorType = 0x21 (HID)
	 * 		bcdHID = 0x0111
	 * 		bCountryCode = Not Supported (0x00)
	 * 		bNumDescriptors = 1
	 * 		bDescriptorType = HID Report (0x22)
	 * 		wDescriptorLength = length
	 *
	 *
	 * INTERFACE DESCRIPTOR (n.0): class HID
	 * 		bLength = 9
	 * 		bDescriptorType = 0x04 (INTERFACE)
	 * 		bInterfaceNumber = n
	 * 		bAlternateSetting = 0
	 * 		bNumEndpoints = bNumEndpoints		1
	 * 		bInterfaceClass = bClass		HID (0x03)
	 * 		bInterfaceSubClass = bSubClass		No Subclass (0x00)
	 * 		bInterfaceProtocol = bProtocol		0x00
	 * 		iInterface = iInterface			20
	 * */
#endif

/*bool sendStringDescriptor2 ( const uint8_t *_string_, uint32_t _maxlen_ );
bool sendDescriptor2 ( USBSetup& setup_ );
void USB_0_Handler ();*/

