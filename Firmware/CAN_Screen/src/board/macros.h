#pragma once

#include "Arduino.h"

//BEGIN - Macros diverses pour la compilation. Copiées depuis le Firmware Marlin pour RepRap.
#define STRINGIFY(s)				# s						// Assemblage textuel				( VAR = 5	STRINGIFY(VAR) => "VAR" )
#define STRINGIFY_(s)				STRINGIFY(s)					// Evaluation de l'assemblage textuel		( VAR = 5	STRINGIFY_(VAR) => "5" )
#define _CAT(a, ...)				a ## __VA_ARGS__
#define SWITCH_ENABLED_false			0
#define SWITCH_ENABLED_true			1
#define SWITCH_ENABLED_0			0
#define SWITCH_ENABLED_1			1
#define SWITCH_ENABLED_0x0			0
#define SWITCH_ENABLED_0x1			1
#define SWITCH_ENABLED_				1
#define ENABLED(b)				_CAT(SWITCH_ENABLED_, b)			// Assemblage					( ENABLED(VAR) => SWITCH_ENABLED_VAR )
#define DISABLED(b)				!ENABLED(b)
//END

//BEGIN - Macros pour lier facilement les horloges, events...
#define _CAT_CLOCK(a, b, c)			a ## b ## c
//#define _CAT_CLOCK_(a, c)			a ## c
//#define GCLK_SYNCBUSY(c)			_CAT_CLOCK_(GCLK_SYNCBUSY_GENCTRL, c)
#define GCLK_SYNCBUSY(c)			_CAT(GCLK_SYNCBUSY_GENCTRL, c)
#define GCLK_PCHCTRL(c)				_CAT_CLOCK(GCLK_PCHCTRL_GEN_GCLK, c, _Val)
//END

//BEGIN - Macros pour différentes conversions.
#define CONV_U16B_TO_CHAR_LE(sub)		( char ) ( sub & MASK_16BLOW ), ( char ) ( ( sub & MASK_16BHIGH ) >> 8 )
#define CONV_U16B_TO_CHAR_BE(sub)		( char ) ( ( sub & MASK_16BHIGH ) >> 8 ), ( char ) ( sub & MASK_16BLOW )
//END

//BEGIN - Différents masques pour une meilleur lecture !
#define MASK_8BITS				0x00000000000000FF
#define MASK_12BITS				0x0000000000000FFF
#define MASK_16BITS				0x000000000000FFFF
#define MASK_16BLOW				MASK_8BITS
#define MASK_16BHIGH				MASK_8BITS << 8
#define MASK_24BITS				0x0000000000FFFFFF
#define MASK_32BITS				0x00000000FFFFFFFF
#define MASK_32BLOW				MASK_16BITS
#define MASK_32BHIGH				MASK_16BITS << 16
#define MASK_32BBYTE1				MASK_8BITS
#define MASK_32BBYTE2				MASK_8BITS << 8
#define MASK_32BBYTE3				MASK_8BITS << 16
#define MASK_32BBYTE4				MASK_8BITS << 24
#define MASK_40BITS				0x000000FFFFFFFFFF
#define MASK_48BITS				0x0000FFFFFFFFFFFF
#define MASK_56BITS				0x00FFFFFFFFFFFFFF
#define MASK_64BITS				0xFFFFFFFFFFFFFFFF
//END

//BEGIN - Pour accéder aux bits de configuration ( Fuses ! )
typedef union {
	struct {
		__I uint32_t TLI:      8;
		__I uint32_t TLD:      4;
		__I uint32_t THI:      8;
		__I uint32_t THD:      4;
		__I uint32_t INT1V1L:  8;
	} bit;
	uint32_t reg;
} FUSES_TEMP0_CALIBRATION_Type;

typedef union {
	struct {
		__I uint32_t INT1V1H:  8;
		__I uint32_t VPL:     12;
		__I uint32_t VPH:     12;
	} bit;
	uint32_t reg;
} FUSES_TEMP1_CALIBRATION_Type;

typedef union {
	struct {
		__I uint32_t VCL:     12;
		__I uint32_t VCH:     12;
		__I uint32_t :         8;
	} bit;
	uint32_t reg;
} FUSES_TEMP2_CALIBRATION_Type;

typedef union {
	struct {
		__I uint32_t :        32;
	} bit;
	uint32_t reg;
} Fuses_TEMP3_CALIBRATION_Type;

typedef struct {
	__I FUSES_TEMP0_CALIBRATION_Type TEMP0;
	__I FUSES_TEMP1_CALIBRATION_Type TEMP1;
	__I FUSES_TEMP2_CALIBRATION_Type TEMP2;
	__I Fuses_TEMP3_CALIBRATION_Type TEMP3;
} FusesCtrl;

#define FUSESCTRL				((FusesCtrl *) 0x00800100UL)
#define FUSESCTRL_INST_NUM			1
#define FUSESCTRL_INSTS				{FUSECTRL}

#define REG_FUSESCTRL_TEMP0			(0x00800100)
#define REG_FUSESCTRL_TEMP1			(0x00800104)
#define REG_FUSESCTRL_TEMP2			(0x00800108)
#define REG_FUSESCTRL_TEMP3			(0x0080010C)

#define TLI					FUSESCTRL->TEMP0.bit.TLI
#define TLD					FUSESCTRL->TEMP0.bit.TLD
#define TL					(float) (TLI + (TLD / (TLD < 10 ? 10 : (TLD < 100 ? 100 : 1000))))
#define THI					FUSESCTRL->TEMP0.bit.THI
#define THD					FUSESCTRL->TEMP0.bit.THD
#define TH					(float) (THI + (THD / (THD < 10 ? 10 : (THD < 100 ? 100 : 1000))))
#define INT1V1L					FUSESCTRL->TEMP0.bit.INT1V1L
#define INT1V1H					FUSESCTRL->TEMP1.bit.INT1V1H
#define VPL					FUSESCTRL->TEMP1.bit.VPL
#define VPH					FUSESCTRL->TEMP1.bit.VPH
#define VCL					FUSESCTRL->TEMP2.bit.VCL
#define VCH					FUSESCTRL->TEMP2.bit.VCH
//END
