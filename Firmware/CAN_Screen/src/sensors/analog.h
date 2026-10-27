#pragma once

#include "Arduino.h"
#include "../board/pins.h"

#define BOX_SENSOR_CHANNEL			g_APinDescription [ BOX_SENSOR ].ulADCChannelNumber
#define WATER_SENSOR_CHANNEL			g_APinDescription [ WATER_TEMP ].ulADCChannelNumber
#define MONITOR_5V_CHANNEL			g_APinDescription [ MONITOR_5V ].ulADCChannelNumber
#define MONITOR_3V3_CHANNEL			g_APinDescription [ MONITOR_3V3 ].ulADCChannelNumber
#define MONITOR_VREF_CHANNEL			g_APinDescription [ MONITOR_VREF ].ulADCChannelNumber
#define MONITOR_VSSA_CHANNEL			g_APinDescription [ MONITOR_VSSA ].ulADCChannelNumber


void ADC_Change_Ref ( uint8_t _ref_ );

void ADC_Init ();

uint16_t ADC_Value ( EAnalogChannel _channel_ );

uint16_t ADC_Internal_Value ( uint8_t _channel_ );

