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
#include "EepromBuffer.h"

static uint8_t          gBitIdx         = 0u;
static uint8_t          gBitBuffer      = 0u;
static uint16_t         gEepromMemAddr  = 0u;

uint16_t  EepromGetMemAddr()
{
  return gEepromMemAddr;
}

uint16_t  EepromGetMemPageAddr()
{
  return (EepromGetMemAddr()-(EepromGetMemAddr()%gcEepromPageSize));
}


static bool EepromBufferWrite(uint8_t data)
{  
  bool res = true;
  if(EEPROM_I2C_write8(gEepromMemAddr,data)==false)
  {
    SignalLED(LED_ERROR);
    res = false;
  }
  gEepromMemAddr++;
  
  if(gEepromMemAddr%gcEepromPageSize==0)
  {
    EepromNewPages(INCREASE);
  
    if(EepromNewPages(GET) >= 452u)  // eeprom nearly full!
    {
      SignalLED(LED_EEPROM);
    }
    
    if(gEepromMemAddr >= gcEepromPages*gcEepromPageSize)  // overflow
    {
      gEepromMemAddr = 0u;
    }
  }
  return res;
}

bool EepromBufferWriteBits(const uint16_t data,const uint8_t bits)
{
  bool res  = true;
  uint8_t i = bits;
  while(i!=0)
  {
    i--;
    if((data & (1u<<i)) != 0u)
    {
      gBitBuffer |= 1u;
    }
    if(gBitIdx == 7u)
    {
      res = EepromBufferWrite(gBitBuffer);
      gBitIdx  = 0u;
      gBitBuffer = 0u;
    }
    else
    {
      gBitIdx++;
      gBitBuffer = gBitBuffer<<1u;
    }
  }
  return res;
}

bool EepromBufferFlash()
{
  return EepromBufferWriteBits(0u,8u-gBitIdx);
}

uint16_t EepromNewPages(NPMODE mode)
{
  static uint16_t         gNewMemPages    = 0u;
  
  if(mode==INCREASE and gNewMemPages<gcEepromPages)
  {
    gNewMemPages++;
  }
  else if(mode==RESET)
  {
    gNewMemPages = 0;
  }
  return gNewMemPages;
}
