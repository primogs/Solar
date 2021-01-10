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
#include "Sensor.h"

static const uint16_t   gMaxADC         = 1023u;

static uint16_t gUbat = 0u;
static uint16_t gUsol = 0u;
static uint16_t gTemp = 0u;
static uint16_t gLum  = 0u;
static LIGHT_PRESCALER gLumPresacler = PRESCALER_OFF;

static uint16_t         ReadAnalog(const int pin);
static void MeasureLight();
static void MeasureTemperature();
static void MeasureUsol();

void InitAnalogeSensors()
{
  pinMode(PIN_U_BAT,INPUT);
  pinMode(PIN_U_SOL,INPUT);
  pinMode(PIN_T_MEAS,INPUT);
  pinMode(PIN_LIGHT,INPUT);

  pinMode(PIN_LUM_PRESCALER,OUTPUT);
  digitalWrite(PIN_LUM_PRESCALER,LOW);
}

uint16_t GetRawUbat()
{
  return gUbat;
}

uint16_t GetRawUsol()
{
  return gUsol;
}

uint16_t GetRawTemp()
{
  return gTemp;
}

uint16_t GetTemperature()
{
  return 0.1064f*static_cast<float>(gTemp)-39.0f;
}

uint16_t GetRawLum()
{
  return gLum;
}

LIGHT_PRESCALER  GetRawLumPresacler()
{
  return gLumPresacler;
}

static uint16_t ReadAnalog(const int pin)
{
  const byte n = 2;
  byte i=0;
  int value[]={0,1};
  for(int j=10;(value[0]!=value[1]) && j>0;j--) // read multiple times until signal is stable
  {
    i++;
    if(i>=n)
      i=0;
    value[i]= analogRead(pin);
    delayMicroseconds(1000*j);
  }
  return value[i];
}

void MeasureSensors()
{
  MeasureTemperature();
  MeasureUsol();
  MeasureUBat();
  MeasureLight();
}

static void MeasureLight()
{
  gLumPresacler = PRESCALER_OFF;
  gLum = ReadAnalog(PIN_LIGHT);
  if(gLum >= gMaxADC)
  {
    digitalWrite(PIN_LUM_PRESCALER,HIGH);
    gLumPresacler = PRESCALER_ON;
    gLum = ReadAnalog(PIN_LIGHT);
    digitalWrite(PIN_LUM_PRESCALER,LOW);
  }
}

static void MeasureUsol()
{
  gUsol = ReadAnalog(PIN_U_SOL);
}

void MeasureUBat()
{
  gUbat = ReadAnalog(PIN_U_BAT);
}

static void MeasureTemperature()
{
  gTemp = ReadAnalog(PIN_T_MEAS);
}



