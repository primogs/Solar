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
#include "ExtEeprom.h"

const uint8_t gcEepromI2CAddr = 0x50u;
const uint8_t gcI2CTimeout = 100u;  // milli seconds

static bool    EEPROM_I2C_write128(uint16_t addr,uint8_t *data); 
static uint8_t EEPROM_I2C_read128(uint16_t addr,uint8_t *data);

bool EEPROM_I2C_begin() 
{
  Wire.beginTransmission(gcEepromI2CAddr);
  if (Wire.endTransmission() == 0) 
  {
    return true;
  }
  return false;
}

bool EEPROM_I2C_write8(uint16_t addr, uint8_t value) 
{ 
  Wire.beginTransmission(gcEepromI2CAddr);
  Wire.write(addr >> 8);
  Wire.write(addr & 0xFF);
  Wire.write(value);
  Wire.endTransmission();

  const unsigned long timeOut = millis()+gcI2CTimeout;
  while (millis()<timeOut) 
  {
    Wire.beginTransmission(gcEepromI2CAddr);
    if (Wire.endTransmission() == 0) 
    {
      return true;
    }
    delay(1);
  }
  return false;
}

uint8_t EEPROM_I2C_read8(uint16_t addr) 
{
  Wire.beginTransmission(gcEepromI2CAddr);
  Wire.write(addr >> 8);
  Wire.write(addr & 0xFF);
  Wire.endTransmission();

  if (1 != Wire.requestFrom(gcEepromI2CAddr, (uint8_t)1)) 
  {
    return 0;
  }
  return Wire.read();
}

bool EEPROM_I2C_write128(uint16_t addr,uint8_t *data) 
{
  const uint8_t dataSize=16;
  Wire.beginTransmission(gcEepromI2CAddr);
  Wire.write(addr >> 8);
  Wire.write(addr & 0xFF);
  Wire.write(data,dataSize);
  Wire.endTransmission();

  // Wait until it acks!
  const unsigned long timeOut = millis()+gcI2CTimeout;
  while (millis()<timeOut)
  {
    Wire.beginTransmission(gcEepromI2CAddr);
    if (Wire.endTransmission() == 0) 
    {
      return true;
    }
    delay(1);
  }
  return false;
}

bool EEPROM_I2C_write(uint16_t addr,uint8_t data[],uint8_t dataSize) 
{
  int i;
  for(i=0;i<(dataSize-15);i+=16,addr+=16)
  {
    EEPROM_I2C_write128(addr,data+i);
  }
  for(;i<dataSize;i++,addr++)
  {
    EEPROM_I2C_write8(addr,data[i]);
  }
  return i;
}


uint8_t EEPROM_I2C_read128(uint16_t addr,uint8_t *data)
{
  const uint8_t dataSize=16;
  Wire.beginTransmission(gcEepromI2CAddr);
  Wire.write(addr >> 8);
  Wire.write(addr & 0xFF);
  Wire.endTransmission();
  Wire.requestFrom(gcEepromI2CAddr, dataSize);
  uint8_t i = 0;
  const unsigned long timeOut = millis()+gcI2CTimeout;
  while (millis()<timeOut and i<dataSize)
  {
    if(Wire.available())
    {
      data[i]= Wire.read();    // receive a byte as character
      i++;
    }
  }
  return i;
}

uint8_t EEPROM_I2C_read(uint16_t addr,uint8_t data[],uint8_t dataSize) 
{
  int i;
  for(i=0;i<(dataSize-15);i+=16,addr+=16)
  {
    EEPROM_I2C_read128(addr,data+i);
  }
  for(;i<dataSize;i++,addr++)
  {
    data[i] = EEPROM_I2C_read8(addr);
  }
  return i;
}

