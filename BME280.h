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

#ifndef BME280_H
#define BME280_H

#include <Wire.h>
#include "Arduino.h"

uint8_t BME280_init(void);
void    BME280_Measure();
float   BME280_readTempAndPressure(void);
float   BME280_readHumidity(void);
float   BME280_readTempC(void);

uint8_t GetRawHousingTemperature();
uint8_t GetRawHumidity();
uint16_t GetRawPressure();

#endif // BME280_H
