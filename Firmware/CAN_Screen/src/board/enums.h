#pragma once

#include "Arduino.h"
#include "./utils.h"

typedef struct {
	union {
		struct {
			uint8_t CAN_Bus: 1;		// Appareil connecté par bus CAN ( Cas normal )
			__I uint8_t : 2;
			uint8_t UART: 1;		// Appareil connecté par l'alternative sériel ( UART )
			uint8_t RS485_Bus: 1;		// Appareil connecté par l'alternative sériel ( RS-485 )
			__I uint8_t : 2;
			uint8_t USB_HID: 1;		// Appareil connecté par le port USB avec un driver HID ( Cas normal )
		};
		uint8_t Connection;
	};

	bool Display;
	bool is_StandAlone ()				{ return ( Connection == 0 ); }
} board_state_t;

typedef struct {
	uint16_t X;					// Position ou dimension suivant X.		Taille:  2 Bytes
	uint16_t Y;					// Position ou dimension suivant Y.		Taille:  2 Bytes
	uint16_t Z;					// Position ou dimension suivant Z.		Taille:  2 Bytes
} space_t;						//						Total:   6 Bytes !

typedef struct {
	int16_t Sensor;					// Le capteur.					Taille:  2 Bytes
	space_t Position;				// La position du capteur.			Taille:  6 Bytes
	space_t Size;					// La taille du capteur.			Taille:  6 Bytes
} external_sensor_t;					//						Total:  14 Bytes !

typedef struct {
	external_sensor_t Input;			// Capteur d'entrée du dissipateur.		Taille: 14 Bytes
	external_sensor_t Output;			// Capteur de sortie du dissipateur.		Taille: 14 Bytes
	space_t Position;				// La position du dissipateur.			Taille:  6 Bytes
	space_t Size;					// La taille du dissipateur.			Taille:  6 Bytes

	int16_t Cooler () const				{ return moy ( this -> Input.Sensor, this -> Output.Sensor ); }
} external_cooler_t;					//						Total:  40 Bytes !

typedef struct {
	union {
		struct {
			int8_t CPU;			// Température processeur.			Taille:  1 Byte
			int8_t GPU1;			// Température carte graphique 1.		Taille:  1 Byte
			int8_t GPU2;			// Température carte graphique 2.		Taille:  1 Byte
		};
		int8_t Temperatures1 [ 3 ];		//						Total:   3 Bytes
	};
	union {
		struct {
			int16_t Rear;			// Température moyenne zone arrière.		Taille:  2 Bytes
			int16_t Top;			// Température moyenne zone supérieure.		Taille:  2 Bytes
			int16_t Front;			// Température moyenne zone frontale.		Taille:  2 Bytes
			int16_t HDD;			// Température moyenne zone disques-durs.	Taille:  2 Bytes
		};
		int16_t Temperatures2 [ 4 ];		//						Total:   8 Bytes
	};
	__I uint8_t Padding;
} external_temperatures_t;				//						Total:  12 Bytes !

typedef struct {
	union {
		struct {
			int16_t Pump;			// Vitesse moyenne des pompes.			Taille:  2 Bytes
			int16_t Rear;			// Vitesse ventilateur arrière.			Taille:  2 Bytes
			int16_t Top;			// Vitesse moyenne ventilateurs supérieurs.	Taille:  2 Bytes
			int16_t Front;			// Vitesse ventilateur frontal.			Taille:  2 Bytes
			int16_t HDD;			// Vitesse moyenne ventilateurs disques-durs.	Taille:  2 Bytes
		};
		int16_t Speeds [ 5 ];			//						Total:  10 Bytes
	};
} external_speeds_t;					//						Total:  10 Bytes !

enum : uint8_t {
	no_feature_ID,
	feature_SensorsTemperature,
	feature_SensorsMonitor,
	feature_HighFlow2Temperature,
	feature_HighFlow2Flow,
	feature_Firmware,
	feature_TimeStamp,
	feature_Clock,
	feature_DayOffset,
	feature_Screen,
	feature_Dialog,
	feature_Reset,
	feature_Max_Count
};

enum : uint8_t {
	TemperatureFeature_ID = 1,
	TemperatureInput_ID,
	MonitorFeature_ID,
	MonitorInput_ID,
	Sensors_TemperatureReport_ID,
	Sensors_MonitorReport_ID,
	HighFlow2_TemperatureReport_ID,
	HighFlow2_FlowReport_ID,
	Firmware_ID = 0x10,
	TimeStamp_ID = 0x20,
	Clock_ID,
	DayOffset_ID,
	Screen_ID = 0x30,
	Dialog_ID,
	Reset_ID = 0xB5
};

enum : uint8_t {
	Sensor_Device = 1,
	HighFlow2_Device,
	Screen_Device
};


