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
#ifndef EEPROM_BUFFER_H
#define EEPROM_BUFFER_H

#include "Arduino.h"
#include "SignalLED.h"
#include "Global.h"

enum NPMODE           {GET,INCREASE,RESET};

uint16_t  EepromNewPages(NPMODE mode);
uint16_t  EepromGetMemAddr();
uint16_t  EepromGetMemPageAddr();
bool      EepromBufferWriteBits(const uint16_t data,const uint8_t bits);
bool      EepromBufferFlash();

#endif // EEPROM_BUFFER_H
