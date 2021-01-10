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
#ifndef EXT_EEPROM_H
#define EXT_EEPROM_H

#include <Wire.h>
#include "Arduino.h"
#include "Global.h"

const uint16_t gcEepromPages = 512u;
const uint16_t gcEepromPageSize = 64u;

bool    EEPROM_I2C_begin();
bool    EEPROM_I2C_write8(uint16_t addr, uint8_t value);
uint8_t EEPROM_I2C_read8(uint16_t addr);
bool    EEPROM_I2C_write(uint16_t addr,uint8_t data[],uint8_t dataSize);
uint8_t EEPROM_I2C_read( uint16_t addr,uint8_t data[],uint8_t dataSize);

#endif // EEPROM_H

