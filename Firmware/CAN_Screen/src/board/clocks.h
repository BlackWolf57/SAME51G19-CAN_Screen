#pragma once

#include "Arduino.h"

#define GENERIC_CLOCK_GENERATOR_MAIN	0				// DFLL @ 120 MHz, généré depuis le FDPLL0 @ 120 MHz !
#define GENERIC_CLOCK_GENERATOR_XOSC32K	3				// Oscillateur interne OSCULP32K @ 32,768 kHz.
#define GENERIC_CLOCK_GENERATOR_1M6	7				// 1,6 MHz, dérivé du crystal externe XOSC0 @ 24 MHz.
#define GENERIC_CLOCK_GENERATOR_12M	4
#define GENERIC_CLOCK_GENERATOR_24M	6				// Crystal externe XOSC0 @ 24 MHz
#define GENERIC_CLOCK_GENERATOR_48M	1
#define GENERIC_CLOCK_GENERATOR_100M	2
#define GENERIC_CLOCK_GENERATOR_120M	8				// FDPLL0 @ 120 MHz, généré depuis le crystal externe XOSC0 @ 24 MHz.

/*
GENERIC_CLOCK_GENERATOR_MAIN    0	 32 kHz, puis 120 MHz ( DPLL0 )
GENERIC_CLOCK_GENERATOR_48M     1	 48 MHz
GENERIC_CLOCK_GENERATOR_100M    2	100 MHz
GENERIC_CLOCK_GENERATOR_XOSC32K 3	 32 kHz
GENERIC_CLOCK_GENERATOR_12M     4	 12 MHz
GENERIC_CLOCK_GENERATOR_1M      5         1 MHz
*/

#define CLOCK_MAIN_FREQ			 48000000
#define CLOCK_1M6_FREQ			  1600000
#define CLOCK_12M_FREQ			 12000000
#define CLOCK_24M_FREQ			 24000000
#define CLOCK_48M_FREQ			 48000000
#define CLOCK_100M_FREQ			100000000
#define CLOCK_120M_FREQ			120000000

//void InitClocks ();
void mdfyClocks ();
/*
static uint8_t _dma_channel = 0;
void DMA_init ();
void DMA_add_channel ( uint8_t _source_, DmacDescriptor *_descriptor_ );
void DMA_enable ();
uint8_t DMA_channels ();
*/
