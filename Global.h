/////////////////////////////////////////////////////////////////////////////////////////
//    This file is part of Solar.
//
//    Copyright (C) 2021 Matthias Hund
//    
//    This program is free software; you can redistribute it and/or
//    modify it under the terms of the GNU General Public License
//    as published by the Free Software Foundation; either version 2
//    of the License, or (at your option) any later version.
//    
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//    
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
/////////////////////////////////////////////////////////////////////////////////////////
#ifndef GLOBAL_H
#define GLOBAL_H

const bool LED_SIG_ENABLE = true;
const bool EEPROM_ENABLE  = true;
const bool CHARGE_ENABLE  = true;
const bool BME280_ENABLE  = true;

#define PIN_UART_RX     0
#define PIN_UART_TX     1
// PIN_ 2 not used
#define PIN_UART_EN     3
#define PIN_LED         4
#define PIN_WLAN_EN     5
#define PIN_SOL_CHARGE  6
#define PIN_LUM_PRESCALER 7
// PIN_ 8 not used

#define PIN_SW_1 9
#define PIN_SW_2 10

//  PIN_ 11 to 13 not used
 
#define PIN_U_SOL   A0
#define PIN_LIGHT   A1
#define PIN_U_BAT   A2
#define PIN_T_MEAS  A3
/* #define PIN_MEM_SDA     A4  
 * #define PIN_MEM_SCL     A5 */
 
#endif
