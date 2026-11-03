#pragma once

#define STRING_IHID_SENSORS_ID			10
#define STRING_IHID_HIGHFLOW2_ID		11
#define STRING_IHID_SCREEN_ID			12

#define CAN_SCREEN_SENSORS_STRING		"CAN Screen - Sensors"
#define CAN_SCREEN_HIGHFLOW2_STRING		"AquaComputer High-Flow 2"
#define CAN_SCREEN_SCREEN_STRING		"CAN Screen - v1"

const uint8_t STRING_IHID_SENSORS_STRING [] = CAN_SCREEN_SENSORS_STRING;
const uint8_t STRING_IHID_HIGHFLOW2_STRING [] = CAN_SCREEN_HIGHFLOW2_STRING;
const uint8_t STRING_IHID_SCREEN_STRING [] = CAN_SCREEN_SCREEN_STRING;

#define STRING_IHID_HIGHFLOW2_FLOW_ID		20

#define CAN_SCREEN_HIGHFLOW2_FLOW_STRING	"Débit du liquide"

//const uint8_t STRING_IHID_HIGHFLOW2_FLOW_STRING [] = CAN_SCREEN_HIGHFLOW2_FLOW_STRING;
const uint8_t CAN_SCREEN_HIGHFLOW2_FLOW_UTF16 [ 32 ] = {
	'D', 0x00, 0xE9, 0x00, 'b', 0x00, 'i', 0x00, 't', 0x00, ' ', 0x00,          // Débit
	'd', 0x00, 'u', 0x00, ' ', 0x00,                                            // du
	'l', 0x00, 'i', 0x00, 'q', 0x00, 'u', 0x00, 'i', 0x00, 'd', 0x00, 'e', 0x00 // liquide
};
