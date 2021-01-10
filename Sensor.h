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
#ifndef SENSOR_H
#define SENSOR_H

#include "Arduino.h"
#include "Global.h"

enum LIGHT_PRESCALER  {PRESCALER_OFF=0,PRESCALER_ON};

void InitAnalogeSensors();

void MeasureSensors();
void MeasureUBat();

uint16_t GetTemperature();
uint16_t GetRawUbat();
uint16_t GetRawUsol();
uint16_t GetRawTemp();
uint16_t GetRawLum();
LIGHT_PRESCALER  GetRawLumPresacler();

#endif // SENSOR_H
