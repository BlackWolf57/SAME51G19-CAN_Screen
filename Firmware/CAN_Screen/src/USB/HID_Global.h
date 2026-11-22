#pragma once

#define VALUE_SIZE_8(x)						(uint8_t) (x)
#define VALUE_SIZE_16_LE(x)					(uint8_t)((x) >>  0), (uint8_t)((x) >>  8)
#define VALUE_SIZE_16_BE(x)					(uint8_t)((x) >>  8), (uint8_t)((x) >>  0)
#define VALUE_SIZE_32_LE(x)					(uint8_t)((x) >>  0), (uint8_t)((x) >>  8), (uint8_t)((x) >> 16), (uint8_t)((x) >> 24)
#define VALUE_SIZE_32_BE(x)					(uint8_t)((x) >> 24), (uint8_t)((x) >> 16), (uint8_t)((x) >>  8), (uint8_t)((x) >>  0)
#define HID_USAGE_EXTENDED_32(a, b)				0x0B, VALUE_SIZE_32_LE((uint32_t)((a) << 16) | (b))


#define COLLECTION_PHYSICAL					0x00
#define COLLECTION_APPLICATION					0x01
#define COLLECTION_LOGICAL					0x02
#define COLLECTION_REPORT					0x03
#define COLLECTION_NAMEDARRAY					0x04
#define COLLECTION_USAGESWITCH					0x05
#define COLLECTION_USAGEMODIFIER				0x06
// Reserved: 0x07 - 0x7F	Future use
// Reserved: 0x80 - 0xFF	Vendor defined


#define HID_DATA						0
#define HID_CONSTANT						1
#define HID_ARRAY						0 << 1
#define HID_VARIABLE						1 << 1
#define HID_ABSOLUTE						0 << 2
#define HID_RELATIVE						1 << 2
#define HID_NOWRAP						0 << 3
#define HID_WRAP						1 << 3
#define HID_LINEAR						0 << 4
#define HID_NONLINEAR						1 << 4
#define HID_PREFERREDSTATE					0 << 5
#define HID_NOPREFERRED						1 << 5
#define HID_NONULLPOSITION					0 << 6
#define HID_NULLSTATE						1 << 6
#define HID_NONVOLATILE						0 << 7
#define HID_VOLATILE						1 << 7
#define HID_BITFIELD						0 << 8
#define HID_BUFFEREDBYTES					1 << 8


#define DV_ABS							(HID_DATA | HID_VARIABLE | HID_ABSOLUTE)
#define DV_REL							(HID_DATA | HID_VARIABLE | HID_RELATIVE)
#define DA_ABS							(HID_DATA | HID_ARRAY | HID_ABSOLUTE)
#define DA_REL							(HID_DATA | HID_ARRAY | HID_RELATIVE)
#define CV_ABS							(HID_CONSTANT | HID_VARIABLE | HID_ABSOLUTE)
#define CV_REL							(HID_CONSTANT | HID_VARIABLE | HID_RELATIVE)
#define CA_ABS							(HID_CONSTANT | HID_ARRAY | HID_ABSOLUTE)
#define CA_REL							(HID_CONSTANT | HID_ARRAY | HID_RELATIVE)

#define UNIT_SYSTEM_NONE					0x00
#define UNIT_SYSTEM_SI_LINEAR					0x01
#define UNIT_SYSTEM_SI_ROTATION					0x02
#define UNIT_SYSTEM_ENGLISH_LINEAR				0x03
#define UNIT_SYSTEM_ENGLISH_ROTATION				0x04
#define UNIT_LENGTH_NONE(a)					((((a) & 0xF) << 4)  | UNIT_SYSTEM_NONE)
#define UNIT_LENGTH_CENTIMETER(a)				((((a) & 0xF) << 4)  | UNIT_SYSTEM_SI_LINEAR)
#define UNIT_LENGTH_RADIANS(a)					((((a) & 0xF) << 4)  | UNIT_SYSTEM_SI_ROTATION)
#define UNIT_LENGTH_INCH(a)					((((a) & 0xF) << 4)  | UNIT_SYSTEM_ENGLISH_LINEAR)
#define UNIT_LENGTH_DEGREES(a)					((((a) & 0xF) << 4)  | UNIT_SYSTEM_ENGLISH_ROTATION)
#define UNIT_MASS_NONE(a)					((((a) & 0xF) << 8)  | UNIT_SYSTEM_NONE)
#define UNIT_MASS_GRAM_SI_LINEAR(a)				((((a) & 0xF) << 8)  | UNIT_SYSTEM_SI_LINEAR)
#define UNIT_MASS_GRAM_SI_ROTATION(a)				((((a) & 0xF) << 8)  | UNIT_SYSTEM_SI_ROTATION)
#define UNIT_MASS_GRAM(a)					((((a) & 0xF) << 8)  | UNIT_SYSTEM_SI_LINEAR)
#define UNIT_MASS_SLUG_ENGLISH_LINEAR(a)			((((a) & 0xF) << 8)  | UNIT_SYSTEM_ENGLISH_LINEAR)
#define UNIT_MASS_SLUG_ENGLISH_ROTATION(a)			((((a) & 0xF) << 8)  | UNIT_SYSTEM_ENGLISH_ROTATION)
#define UNIT_MASS_SLUG(a)					((((a) & 0xF) << 8)  | UNIT_SYSTEM_ENGLISH_LINEAR)
#define UNIT_TIME_NONE(a)					((((a) & 0xF) << 12) | UNIT_SYSTEM_NONE)
#define UNIT_TIME_SECONDS_SI_LINEAR(a)				((((a) & 0xF) << 12) | UNIT_SYSTEM_SI_LINEAR)
#define UNIT_TIME_SECONDS_SI_ROTATION(a)			((((a) & 0xF) << 12) | UNIT_SYSTEM_SI_ROTATION)
#define UNIT_TIME_SECONDS_ENGLISH_LINEAR(a)			((((a) & 0xF) << 12) | UNIT_SYSTEM_ENGLISH_LINEAR)
#define UNIT_TIME_SECONDS_ENGLIS_ROTATION(a)			((((a) & 0xF) << 12) | UNIT_SYSTEM_ENGLISH_ROTATION)
#define UNIT_TIME_SECONDS(a)					((((a) & 0xF) << 12) | UNIT_SYSTEM_SI_LINEAR)
#define UNIT_TEMPERATURE_NONE(a)				((((a) & 0xF) << 16) | UNIT_SYSTEM_NONE)
#define UNIT_TEMPERATURE_KELVIN_SI_LINEAR(a)			((((a) & 0xF) << 16) | UNIT_SYSTEM_SI_LINEAR)
#define UNIT_TEMPERATURE_KELVIN_SI_ROTATION(a)			((((a) & 0xF) << 16) | UNIT_SYSTEM_SI_ROTATION)
#define UNIT_TEMPERATURE_KELVIN(a)				((((a) & 0xF) << 16) | UNIT_SYSTEM_SI_LINEAR)
#define UNIT_TEMPERATURE_FAHRENHEIT_ENGLISH_LINEAR(a)		((((a) & 0xF) << 16) | UNIT_SYSTEM_ENGLISH_LINEAR)
#define UNIT_TEMPERATURE_FAHRENHEIT_ENGLISH_ROTATION(a)		((((a) & 0xF) << 16) | UNIT_SYSTEM_ENGLISH_ROTATION)
#define UNIT_TEMPERATURE_FAHRENHEIT(a)				((((a) & 0xF) << 16) | UNIT_SYSTEM_ENGLISH_LINEAR)
#define UNIT_CURRENT_NONE(a)					((((a) & 0xF) << 20) | UNIT_SYSTEM_NONE)
#define UNIT_CURRENT_AMPERE_SI_LINEAR(a)			((((a) & 0xF) << 20) | UNIT_SYSTEM_SI_LINEAR)
#define UNIT_CURRENT_AMPERE_SI_ROTATION(a)			((((a) & 0xF) << 20) | UNIT_SYSTEM_SI_ROTATION)
#define UNIT_CURRENT_AMPERE_ENGLISH_LINEAR(a)			((((a) & 0xF) << 20) | UNIT_SYSTEM_ENGLISH_LINEAR)
#define UNIT_CURRENT_AMPERE_ENGLISH_ROTATION(a)			((((a) & 0xF) << 20) | UNIT_SYSTEM_ENGLISH_ROTATION)
#define UNIT_CURRENT_AMPERE(a)					((((a) & 0xF) << 20) | UNIT_SYSTEM_SI_LINEAR)
#define UNIT_LUMINOUS_INTENSITY_NONE(a)				((((a) & 0xF) << 24) | UNIT_SYSTEM_NONE)
#define UNIT_LUMINOUS_INTENSITY_CANDELA_SI_LINEAR(a)		((((a) & 0xF) << 24) | UNIT_SYSTEM_SI_LINEAR)
#define UNIT_LUMINOUS_INTENSITY_CANDELA_SI_ROTATION(a)		((((a) & 0xF) << 24) | UNIT_SYSTEM_SI_ROTATION)
#define UNIT_LUMINOUS_INTENSITY_CANDELA_ENGLISH_LINEAR(a)	((((a) & 0xF) << 24) | UNIT_SYSTEM_ENGLISH_LINEAR)
#define UNIT_LUMINOUS_INTENSITY_CANDELA_ENGLISH_ROTATION(a)	((((a) & 0xF) << 24) | UNIT_SYSTEM_ENGLISH_ROTATION)
#define UNIT_LUMINOUS_INTENSITY(a)				((((a) & 0xF) << 24) | UNIT_SYSTEM_SI_LINEAR)
#define UNIT_NIBBLES(n0,n1,n2,n3,n4,n5,n6)			(((n6 & 0xF) << 24) | ((n5 & 0xF) << 20) | ((n4 & 0xF) << 16) | ((n3 & 0xF) << 12) | ((n2 & 0xF) << 8) | ((n1 & 0xF) << 4) | (n0 & 0xF))

#define UNIT_EXPONENT_PLUS_5					0x5		// 10^5  (Centaines de milliers)
#define UNIT_EXPONENT_PLUS_6					0x6		// 10^6  (Méga)
#define UNIT_EXPONENT_PLUS_7					0x7		// 10^7  (Dix Méga)
#define UNIT_EXPONENT_MINUS_8					0x8		// 10^-8 (Cent-millionièmes / Cent-micro)
#define UNIT_EXPONENT_MINUS_7					0x9		// 10^-7 (Dix-millionièmes / Dix-micro)
#define UNIT_EXPONENT_MINUS_6					0xA		// 10^-6 (Micro)
#define UNIT_EXPONENT_MINUS_5					0xB		// 10^-5 (Cent-milli)
#define UNIT_EXPONENT_MINUS_4					0xC		// 10^-4 (Dix-milli)
#define UNIT_EXPONENT_MINUS_3					0xD		// 10^-3 (Milli)
#define UNIT_EXPONENT_MINUS_2					0xE		// 10^-2 (Centi)
#define UNIT_EXPONENT_MINUS_1					0xF		// 10^-1 (Déci)
#define UNIT_EXPONENT_0						0x0		// 10^0  (Unité de base)
#define UNIT_NULL						0x0

#define HID_USAGE_PAGE(a)					0x05, a
#define HID_USAGE_PAGE16(a)					0x06, VALUE_SIZE_16_LE(a)
#if defined(__linux__)
	#define HID_USAGE(a)					(defined(HID_EXTENDED_PAGE_ACTIVE) ? (0x0B, VALUE_SIZE_32_LE((uint32_t) ((HID_EXTENDED_PAGE_ACTIVE) << 16) | (a))) : (0x09, a))
	#define HID_USAGE16(a)					(defined(HID_EXTENDED_PAGE_ACTIVE) ? (0x0B, VALUE_SIZE_32_LE((uint32_t) ((HID_EXTENDED_PAGE_ACTIVE) << 16) | (a))) : (0x0A, VALUE_SIZE_16_LE(a)))
	#define HID_USAGE16_BE(a)				(defined(HID_EXTENDED_PAGE_ACTIVE) ? (0x0B, VALUE_SIZE_32_BE((uint32_t) ((HID_EXTENDED_PAGE_ACTIVE) << 16) | (a))) : (0x0A, VALUE_SIZE_16_BE(a)))
#else
	#define HID_USAGE(a)					0x09, a
	#define HID_USAGE16(a)					0x0A, VALUE_SIZE_16_LE(a)
	#define HID_USAGE16_BE(a)				0x0A, VALUE_SIZE_16_BE(a)
#endif
#define HID_USAGE32(a)						0x0B, VALUE_SIZE_32_LE(a)
#define HID_USAGE32_BE(a)					0x0B, VALUE_SIZE_32_BE(a)
#define HID_LOGICAL_MIN8(a)					0x15, a
#define HID_LOGICAL_MIN16(a)					0x16, VALUE_SIZE_16_LE(a)
#define HID_LOGICAL_MIN32(a)					0x17, VALUE_SIZE_32_LE(a)
#define HID_USAGE_MIN8(a)					0x19, a
#define HID_USAGE_MIN16(a)					0x1A, VALUE_SIZE_16_LE(a)
#define HID_LOGICAL_MAX8(a)					0x25, a
#define HID_LOGICAL_MAX16(a)					0x26, VALUE_SIZE_16_LE(a)
#define HID_LOGICAL_MAX32(a)					0x27, VALUE_SIZE_32_LE(a)
#define HID_USAGE_MAX8(a)					0x29, a
#define HID_USAGE_MAX16(a)					0x2A, VALUE_SIZE_16_LE(a)
#define HID_PHYSICAL_MIN8(a)					0x35, a
#define HID_PHYSICAL_MIN16(a)					0x36, VALUE_SIZE_16_LE(a)
#define HID_PHYSICAL_MIN32(a)					0x37, VALUE_SIZE_32_LE(a)
#define HID_PHYSICAL_MAX8(a)					0x45, a
#define HID_PHYSICAL_MAX16(a)					0x46, VALUE_SIZE_16_LE(a)
#define HID_PHYSICAL_MAX32(a)					0x47, VALUE_SIZE_32_LE(a)
#define HID_UNIT_EXPONENT(a)					0x55, a
#define HID_UNIT_TYPE8(a)					0x65, a
#define HID_UNIT_TYPE16(a)					0x66, VALUE_SIZE_16_LE(a)
#define HID_UNIT_TYPE32(a)					0x67, VALUE_SIZE_32_LE(a)
#define HID_REPORT_SIZE(a)					0x75, a
#define HID_REPORT_SIZE16(a)					0x76, VALUE_SIZE_16_LE(a)
#define HID_STRING_INDEX8(a)					0x79, a
#define HID_STRING_INDEX16(a)					0x7A, VALUE_SIZE_16_LE(a)
#define HID_INPUT(a)						0x81, a
#define HID_REPORT_ID(a)					0x85, a
#define HID_STRING_MIN8(a)					0x89, a
#define HID_STRING_MIN16(a)					0x8A, VALUE_SIZE_16_LE(a)
#define HID_OUTPUT(a)						0x91, a
#define HID_REPORT_COUNT(a)					0x95, a
#define HID_REPORT_COUNT16(a)					0x96, VALUE_SIZE_16_LE(a)
#define HID_STRING_MAX8(a)					0x99, a
#define HID_STRING_MAX16(a)					0x9A, VALUE_SIZE_16_LE(a)
#define HID_COLLECTION(a)					0xA1, a
#define HID_FEATURE(a)						0xB1, a
#define HID_END_COLLECTION					0xC0
#define HID_USAGE_SENSOR_DATA(a,b)				(a | b)
#define HID_PUSH						0xA4
#define HID_POP							0xB4
#define HID_DELIMITER_OPEN					0xA9, 0x01
#define HID_DELIMITER_CLOSE					0xA9, 0x00

