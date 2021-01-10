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
#include "SignalLED.h"

void SignalLED(const LED_SIGNAL sig)
{
  if(LED_SIG_ENABLE)
  {
    Serial.flush();
    switch(sig)
    {
      case LED_INIT:
      {
        digitalWrite(PIN_LED,HIGH);
        LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
        digitalWrite(PIN_LED,LOW);
      }
      break;
      case LED_EEPROM:
      {
        digitalWrite(PIN_LED,HIGH);
        LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
        digitalWrite(PIN_LED,LOW);
        LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);
        digitalWrite(PIN_LED,HIGH);
        LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);
        digitalWrite(PIN_LED,LOW);
      }
      break;
      case LED_ERROR:
      {
        digitalWrite(PIN_LED,HIGH);
        LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
        digitalWrite(PIN_LED,LOW);
        LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
        digitalWrite(PIN_LED,HIGH);
        LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
        digitalWrite(PIN_LED,LOW);
        LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);
        digitalWrite(PIN_LED,LOW);
        LowPower.powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF);
        digitalWrite(PIN_LED,LOW);
        LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
        digitalWrite(PIN_LED,LOW);
      }
      break;
      case LED_BLINK:
      default:
      {
        digitalWrite(PIN_LED,HIGH);
        LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
        digitalWrite(PIN_LED,LOW);
      }
      break;
      case LED_CHARGE_BLINK:
      {
          digitalWrite(PIN_LED,HIGH);
          LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
          digitalWrite(PIN_LED,LOW);
          LowPower.powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF);
          digitalWrite(PIN_LED,HIGH);
          LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
          digitalWrite(PIN_LED,LOW);
          LowPower.powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF);
        digitalWrite(PIN_LED,HIGH);
        LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
        digitalWrite(PIN_LED,LOW);
      }
      break;
    }
  }
}

