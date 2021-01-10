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
#include "BME280.h"
#include "BlueDot_BME280.h"
BlueDot_BME280 bme280 = BlueDot_BME280();

static uint8_t gHoT = 0u;
static uint8_t gHum = 0u;
static uint16_t gPre = 0u;
  
uint8_t BME280_init(void)
{
  bme280.parameter.communication = 0;
  bme280.parameter.I2CAddress = 0x76;
  bme280.parameter.sensorMode = 0b11;
  bme280.parameter.IIRfilter = 0b100; 
  bme280.parameter.humidOversampling = 0b101; 
  bme280.parameter.tempOversampling = 0b101;
  bme280.parameter.pressOversampling = 0b101;
  return (bme280.init() == 0x60);
}

void    BME280_Measure()
{
}

uint8_t GetRawHousingTemperature()
{
  return gHoT;
}

uint8_t GetRawHumidity()
{
  return gHum;
}

uint16_t GetRawPressure()
{
  return gPre;  
}

float BME280_readTempAndPressure(void)
{
    BME280_readTempC();

    float P = bme280.readPressure();
    
    if(P >= 900.0f and P <= 1100.0f)
      gPre = (uint16_t) ((P-900.0f)/20.0f);
    
    return P;
}


float BME280_readHumidity(void)
{
    float H = bme280.readHumidity();
    if(H >= 0.0f and H <= 100.0f)
      gHum = (uint8_t)(H*2.0f);
      
    return H;
}

float BME280_readTempC(void)
{  
    float T = bme280.readTempC();
    if(T >= -40.0f and T <= 85.0f)
      gHoT = (uint8_t)((T+40.0f)*2.0f);
      
    return T;
}


