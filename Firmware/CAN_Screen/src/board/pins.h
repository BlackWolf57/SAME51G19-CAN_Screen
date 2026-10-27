/**
 * Custom Rhéobus.
 * Screen interface
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#pragma once

//BEGIN - Ce qui est branché sur les bornes PWM.
#define INTERRUPTION	 0						// PA.08
#define BOX_SENSOR	 2						// PA.10	ADC0 [ 10 ]
#define FLOW_SENSOR	 3						// PA.11	TC1 [ 1 ]		TCC0 [ 3 ]		TCC1 [ 7 ]
#define SCREEN_RESET	 4						// PB.10
#define SCREEN_RS	 5						// PB.11
#define I2C_SDA		 8						// PA.16	SERCOM1 [ 0 ]
#define I2C_SCL		 9						// PA.17	SERCOM1 [ 1 ]
#define SCREEN_DB6	10						// PA.18
#define SCREEN_DB7	11						// PA.19
#define SWCLK		12						// PA.20
#define SWDIO		13						// PA.21
//END

//BEGIN - Ce qui est branché sur les bornes de communication.
#define SCREEN_RW	20						// PA.12
#define SCREEN_EN	21						// PA.13
//END

//BEGIN - Ce qui est branché sur les bornes digitales.
#define SCREEN_DB0	23						// PA.00
#define COMM_RTS	24						// PA.30	SERCOM3 [ 3 ]
#define WATER_TEMP	26						// PB.02	ADC0 [ 14 ]
//END

//BEGIN - Ce qui est branché sur les bornes analogiques.
#define MONITOR_5V	54						// PA.02	ADC0 [ 0 ]
#define MONITOR_3V3	55						// PA.03	ADC0 [ 1 ]
#define MONITOR_VREF	56						// PB.08	ADC0 [ 2 ]		ADC1 [ 0 ]
#define MONITOR_VSSA	57						// PB.09	ADC0 [ 3 ]		ADC1 [ 1 ]
#define SCREEN_DB3	58						// PA.04
#define SCREEN_DB2	59						// PA.05
#define COMM_SELECT1	60						// PA.06
#define COMM_SELECT2	61						// PA.07
//END

//BEGIN - Bus CAN / Communications.
#define COMM_TX		68						// PA.22	SERCOM3 [ 0 ]
#define COMM_RX		69						// PA.23	SERCOM3 [ 1 ]
//END

//BEGIN - Ce qui est branché sur le port SPI.
#define SCREEN_DB5	74						// PB.22
#define SCREEN_DB4	75						// PB.23
#define SCREEN_DB1	76						// PA.01
//END

//BEGIN - Port USB natif.
//#define USB_MINUS	86						// PA.24
//#define USB_PLUS	87						// PA.25
//END

