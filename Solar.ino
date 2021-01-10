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

#include <LowPower.h>
#include <avr/wdt.h>

#include "ExtEeprom.h"
#include "EepromBuffer.h"
#include "BME280.h"
#include "Sensor.h"
#include "Global.h"
  
#define BAT_OVERFULL_VOLTAGE        2.45f
#define BAT_FULL_VOLTAGE            2.125f
#define BAT_EMPTY_VOLTAGE           2.0f
#define BAT_RECHARGE_LOW_VOLTAGE    2.032f
#define BAT_RECHARGE_HIGH_VOLTAGE   2.092f
#define SOL_LOW_LIGHT_VOLTAGE       3.0f
#define SOL_NO_LIGHT_VOLTAGE        1.0f

enum PWR_EVENT        {BAT_NORMAL,BAT_CHARGEING,BAT_FULL,BAT_OVER_VOLTAGE};
enum CMOS_STATE       {M_OFF,M_ON};

static bool     gOverVoltageFlag    = false;
static bool     gHangUpFlag         = false;
static bool     gForceUpload        = false;

static PWR_EVENT        gPowerStatus    = BAT_NORMAL;
static uint8_t          gWlanErr        = 0u;

// ############################################################################################################################
// ##### function declaration #####
// sensor
static float            GetUbat();
static float            GetUsol();

// power management
static void             SetSState(const CMOS_STATE state);
static float            ChargeLevel(const float Ubat);
static void             BatManagement();
static void             PowerManagement();

// memory
static void             WhatNeedsToBeWritten(bool &bWriteLight, bool &bWritePresureAndUsol, bool &bWriteTemperature, bool &bWriteOther, bool &bWriteAlignBits);
static void             WriteEeprom();
static void             WriteLight();
static void             WritePresureAndUsol();
static void             WriteTemperature();
static void             WriteOther();

// CRC
static inline bool      GetBit(const unsigned char bits[],const int n);
static uint16_t         CRC16 (const unsigned char bits[],const int n);

// communication
static void             SerialFlushInput();
static void             PrintRawValues();
static void             TransmitBlock(const uint16_t page,bool verbose_mode=false);
static char *           ReadFromSerial();
static void             EnterDebugMode();
static bool             EnterUploadMode();
static void             DataUpload();
static void             CheckSwitches();

// ############################################################################################################################
// ##### implementation of sensor functions #####


static float GetUbat()
{
  return 0.0036362f*GetRawUbat();   // ADC=580 at Usol=2,109 V
}

static float GetUsol()
{
  return 0.0057767f*GetRawUsol(); // ADC=618 at Usol=3,57 V
}

// ##### implementation of power management functions #####

static void SetSState(const CMOS_STATE state)
{
  switch(state)
  {
    case M_OFF:
    {
      digitalWrite(PIN_SOL_CHARGE,LOW);
      break;
    }
    case M_ON:
    {
      digitalWrite(PIN_SOL_CHARGE,HIGH);
      break;
    }
  }
}
/*
 * return the charge level of the battery. Valid only if no current is drawn.
 * 0.0 = battery is empty
 * 1.0 = battery is fully charged
 */
static float ChargeLevel(const float Ubat)
{
  float level = (Ubat-BAT_EMPTY_VOLTAGE)/(BAT_FULL_VOLTAGE-BAT_EMPTY_VOLTAGE);
  if(level >1.0f)
    level=1.0f;
  else if(level<0.0f)
    level = 0.0f;
  return level ;
}

float GetBatteryRechargeVoltage()
{
  float voltage = (BAT_RECHARGE_LOW_VOLTAGE+BAT_RECHARGE_HIGH_VOLTAGE)/2.0f*(1.0f+(GetTemperature()-25.0f)*1.16E-3f);
  if(voltage<BAT_RECHARGE_LOW_VOLTAGE)
    voltage=BAT_RECHARGE_LOW_VOLTAGE;
  if(voltage>BAT_RECHARGE_HIGH_VOLTAGE)
    voltage=BAT_RECHARGE_HIGH_VOLTAGE;
  return voltage;
}

static void BatManagement()
{
  static uint32_t   gToCharge         = 0;
  static int        endChargeCounter  = 0;
  const float       Ubat = GetUbat();
  const float       Usol = GetUsol();

  if(Usol<SOL_NO_LIGHT_VOLTAGE) // reset flag if it is dark
  {
    gOverVoltageFlag  = false;
  }

  switch(gPowerStatus) // batterie state maschine
  {
    case BAT_NORMAL:
    { // begin to charge battery if
      if( Ubat <   GetBatteryRechargeVoltage() and      // battery voltage is below threshold
          gOverVoltageFlag  ==  false and               // overvoltage flag is reset
          Usol              > SOL_LOW_LIGHT_VOLTAGE and // sun is shining
          CHARGE_ENABLE)                                // charging is enabled
      {
        gToCharge = (uint32_t)((1.0f-ChargeLevel(Ubat))*2500.0*3600.0); // estimate charge, Unit: mA s
        gPowerStatus = BAT_CHARGEING;
      }
    }
    break;
    case BAT_CHARGEING:
    {
      if(Ubat>BAT_OVERFULL_VOLTAGE) // battery overvoltage?
      {
        endChargeCounter++;
        if(endChargeCounter>3)
        {
          endChargeCounter=0;
          gPowerStatus=BAT_OVER_VOLTAGE;
        }
      }
      else if(Usol<SOL_NO_LIGHT_VOLTAGE)
      {
        gPowerStatus=BAT_NORMAL;
      }
      else
      {  
        uint32_t cCharge = 50.0f*9.0f;  // charge current ((3.3V-2.1V-0.65V)/11 Ohm = 50 mA) x time between calls
        if(gToCharge>cCharge)
        {
          gToCharge-=cCharge;
        }
        else
        {
          gPowerStatus=BAT_FULL;
          gToCharge=0;
        }
      }
    }
    break;
    case BAT_OVER_VOLTAGE:
    {
      gOverVoltageFlag = true;
      gPowerStatus = BAT_NORMAL;
    }
    break;
    case BAT_FULL:
    {
      gPowerStatus = BAT_NORMAL;
    }
    break;
  }
}

static void PowerManagement()
{
  BatManagement();
  if(gPowerStatus==BAT_CHARGEING) // switch solar panel on to charge battery
  {
    SetSState(M_ON);
  }
  else
  {
    SetSState(M_OFF);
  }
}

// ##### implementation of memory functions #####
static void WriteLight()
{
  EepromBufferWriteBits(GetRawLum()         ,10u);
  EepromBufferWriteBits(GetRawLumPresacler(),2u);
}

static void WritePresureAndUsol()
{
  BME280_readTempAndPressure();

  EepromBufferWriteBits(GetRawUsol()    ,10u);
  EepromBufferWriteBits(GetRawPressure(),10u);
}

static void WriteTemperature()
{
  EepromBufferWriteBits(GetRawTemp(),10u);
}

static void WriteOther()
{
  BME280_readHumidity();
  EepromBufferWriteBits(GetRawHumidity()          ,8u);
  EepromBufferWriteBits(GetRawHousingTemperature(),8u);
  EepromBufferWriteBits(gPowerStatus              ,2u);
  EepromBufferWriteBits(GetRawUbat()              ,10u);
}

static void WhatNeedsToBeWritten(bool &bWriteLight, bool &bWritePresureAndUsol, bool &bWriteTemperature, bool &bWriteOther,bool &bWriteAlignBits)
{
  static uint8_t callCount = 1u;
  if(callCount==201)
  {
    callCount=1u;
    bWriteAlignBits = true;
  }
  
  if(callCount%10u == 0)
  {
    bWriteLight=true;
    if(callCount%20u == 0)
    {
      bWritePresureAndUsol=true;
    }
    if(callCount%50u == 0)
    {
      bWriteTemperature=true;
      if(callCount == 200)
      {
        bWriteOther = true;
      }
    }
  }
  callCount++;
}

static void WriteEeprom()
{
  bool bWriteLight          = false;
  bool bWritePresureAndUsol = false;
  bool bWriteTemperature    = false;
  bool bWriteOther          = false;
  bool bWriteAlignBits      = false;

  WhatNeedsToBeWritten(bWriteLight,bWritePresureAndUsol,bWriteTemperature,bWriteOther,bWriteAlignBits);

  if(bWriteAlignBits)
  {
    EepromBufferFlash();
  }
  
  if(bWriteLight)
  {
    WriteLight();
  }

  if(bWritePresureAndUsol)
  {
    BME280_Measure();
    WritePresureAndUsol();
  }

  if(bWriteTemperature)
  {
    WriteTemperature();
  }

  if(bWriteOther)
  {
    WriteOther();
  }

}

// ##### implementation of CRC functions #####
static inline bool GetBit(const unsigned char bits[],const int n)
{
  int by=n/8;
  int br=n%8;
  return ((bits[by] >> (7-br)) bitand 1);
}

static uint16_t CRC16 (const unsigned char bits[],const int n)
{
  const unsigned int short CRC16MASK = 0x1021;  // CRC-32 Bitmaske 
  unsigned int short crc16       = 0xffff;    // shift register 
  for (int i = 0; i < n*8; i++)
  {
    if ( ((crc16 >> 15) & 1) != GetBit(bits,i))
      crc16 = (crc16 << 1) ^ CRC16MASK;
    else
      crc16 = (crc16 << 1);
  }
  return crc16;
}

// ##### implementation of communication functions #####

static void SerialFlushInput()
{
  while(Serial.available())
  {
    Serial.read();
  }
}

static void PrintRawValues()
{
  Serial.print(gPowerStatus);
  Serial.print(' ');
  Serial.print(GetRawUsol());
  Serial.print(' ');
  Serial.print(GetRawUbat());
  Serial.print(' ');
  Serial.print(GetRawTemp());
  Serial.print(' ');
  Serial.println(GetRawLum());
}

static void TransmitBlock(const uint16_t page,bool verbose_mode)
{
  const uint8_t crcByteSize   = 2u;
  const uint8_t pageByteSize  = 2u;
  uint8_t blkData[gcEepromPageSize+pageByteSize+crcByteSize];
  uint16_t      varSize       = 0;
  uint16_t       addr          = (page+1u)*gcEepromPageSize;
  const uint16_t current_addr = EepromGetMemPageAddr();
  if(current_addr>=addr)
    addr = current_addr-addr;
  else
    addr = (gcEepromPageSize*gcEepromPages)-(addr-current_addr);
  
  varSize = gcEepromPageSize;
  EEPROM_I2C_read(addr,blkData,varSize); // read page from eeprom

  memcpy(&(blkData[varSize]),&addr,pageByteSize); // append eeprom address

  varSize = gcEepromPageSize+pageByteSize;          // append crc sum
  const uint16_t crcSum = CRC16(blkData,varSize);
  memcpy(&(blkData[varSize]),&crcSum,crcByteSize);
  
  Serial.write(blkData,sizeof(blkData));

  if(verbose_mode)
  {
    Serial.println("");
    Serial.print("page ");
    Serial.println(page);
    Serial.print("mem pointer ");
    Serial.println(EepromGetMemAddr());
    Serial.print("addr ");
    Serial.println(addr);
    Serial.print("crc ");
    Serial.println(crcSum,HEX);
    for(unsigned int i=0;i<sizeof(blkData);i++)
    {
      Serial.print(blkData[i],HEX);
      Serial.print(" ");
    }
    Serial.println("");
  }
}

static char * ReadFromSerial()
{
  static char msg[16];
  static unsigned char index = 0;
  char * res = NULL;
  msg[index] = Serial.read();
  index++;
  if(index == sizeof(msg) or msg[index-1]=='\n' or msg[index-1]=='\r')
  {
    msg[index-1] = '\0';
    res = msg;

    index=0;
  } 
  return res;
}

static void EnterDebugMode()
{
  static int eepromWritePointer = 0;
  bool runLoop = true;
  while(runLoop)
  {
    while(Serial.available()>0)
    {
      char *msg = ReadFromSerial();
      if(msg != NULL)
      {
        if(strcmp(msg,"CON")==0)
        {
          SetSState(M_ON);
          Serial.println("charge on");
        }
        else if(strcmp(msg,"COF")==0)
        {
          SetSState(M_OFF);
          Serial.println("charge off");
        }
        else if(strcmp(msg,"WON")==0)
        {
          digitalWrite(PIN_WLAN_EN,LOW);
          Serial.println("wifi on");
        }
        else if(strcmp(msg,"WOF")==0)
        {
          digitalWrite(PIN_WLAN_EN,HIGH);
          Serial.println("wifi off");
        }
        else if(strncmp(msg,"QTY",3)==0)
        {
          const uint16_t newPages= EepromNewPages(GET);
          if(newPages==0)
            Serial.println("-1");
          else
            Serial.println(newPages);
        }
        else if(strncmp(msg,"GET",3)==0)
        {
          int pageNr = atoi(&(msg[3]));
          if(pageNr >= 0 and (uint16_t)pageNr < gcEepromPages)
          {
            TransmitBlock(pageNr,true);
          }
        }
        else if(strncmp(msg,"VAL",3)==0)
        {
          MeasureSensors();
          PrintRawValues();
        }
        else if(strncmp(msg,"REP",3)==0)
        {
          int addr = atoi(&(msg[3]));
          uint8_t data = EEPROM_I2C_read8(addr);
          Serial.println(data,HEX);
        }
        else if(strncmp(msg,"WEP",3)==0)
        {
          int value = atoi(&(msg[3]));
          if(EEPROM_I2C_write8(eepromWritePointer,value)==false)
          {
            Serial.println("timeout");
          }
        }
        else if(strncmp(msg,"SEP",3)==0)
        {
          eepromWritePointer = atoi(&(msg[3]));
          Serial.println("eepromWritePointer");
        }
        else if(strncmp(msg,"WPG",3)==0)
        {
          Serial.println("write eeprom page not implemented");
        }
        else if(strncmp(msg,"ZPG",3)==0)
        {
          Serial.println("zero eeprom page");
          uint8_t zeros[64];
          memset(zeros,0,sizeof(zeros));
          bool res = EEPROM_I2C_write(0,zeros,sizeof(zeros));
          if(res)
            Serial.println("zeroing successful");
          else
            Serial.println("zeroing failed");
        }
        else if(strncmp(msg,"RPG",3)==0)
        {
          uint8_t data[64];
          uint8_t n = EEPROM_I2C_read(0,data,sizeof(data)); 
          Serial.print("read ");
          Serial.print(n);
          Serial.println(" bytes");
          for(int i=0;i<n;i++)
          {
            Serial.print(data[i],HEX);
            Serial.print(" ");
          }
          Serial.println();
        }
        else if(strncmp(msg,"BME",3)==0)
        {
          BME280_Measure();
          float pressure    = BME280_readTempAndPressure();
          float humidity    = BME280_readHumidity();
          float temperature = BME280_readTempC();
          Serial.print("p: ");
          Serial.print(pressure);
          Serial.print(" h: ");
          Serial.print(humidity);
          Serial.print(" T: ");
          Serial.println(temperature);
        }
      }
      
      if(digitalRead(PIN_SW_1)==HIGH)
      {
         runLoop=false;
      }
      
      delay(100);
    }
  }
  Serial.println("leaving debug mode");
}

static void DataUpload()
{
  if((EepromNewPages(GET) > 12 and 
      ChargeLevel(GetUbat())>0.25f and 
      GetUsol()>SOL_LOW_LIGHT_VOLTAGE ) or 
      (gForceUpload and EepromNewPages(GET)>0))
  {
    EnterUploadMode();
  }
}

static bool EnterUploadMode()
{
  wdt_disable();

  bool finished = false;
  bool res = false;
  SetSState(M_ON);

  digitalWrite(PIN_UART_EN,HIGH);
  digitalWrite(PIN_WLAN_EN,LOW);
  delay(140); // wait until ESP has booted
  Serial.begin(19200);

  SerialFlushInput();

  const unsigned long timeOut       = 10000u;   // 10 seconds
  const unsigned long totalTimeOut  = 150000u; // 2,5 minutes
  unsigned long waitTime  = millis()+timeOut;
  unsigned long endTime   = millis()+totalTimeOut;
  
  while(millis()<endTime and !finished)
  {
    if(millis()>waitTime) // try power cycle
    {
      digitalWrite(PIN_WLAN_EN,HIGH);
      delay(timeOut);
      MeasureUBat();
      if(GetUbat()<BAT_EMPTY_VOLTAGE) // check battery voltage
      {
        finished = true;
      }
      else
      {
        digitalWrite(PIN_WLAN_EN,LOW);
        delay(140);   // wait until ESP has boot
        Serial.flush();
      }
    }
    
    while(Serial.available()>0 and !finished)
    {
      char *msg = ReadFromSerial();
      if(msg != NULL)
      {
        if(strncmp(msg,"QTY",3)==0)
        {
          const uint16_t newPages= EepromNewPages(GET);
          if(newPages==0)
            Serial.println("-1");
          else
            Serial.println(newPages);
        }
        else if(strncmp(msg,"GET",3)==0)
        {
          int pageNr = atoi(&(msg[3]));
          if(pageNr >= 0 and (uint16_t)pageNr < gcEepromPages)
          {
            TransmitBlock(pageNr);
          }
        }
        else if(strncmp(msg,"END",3)==0)
        {
          EepromNewPages(RESET);
          finished = true;
          res = true;
          delay(100);
        }
        else if(strncmp(msg,"ERR",3)==0)
        {
          gWlanErr = atoi(&(msg[3]));
          finished = true;
          delay(100);
        }
        waitTime = millis()+timeOut;
      }
    }

  }
  Serial.end();
  digitalWrite(PIN_UART_EN,LOW);
  digitalWrite(PIN_WLAN_EN,HIGH);

  return res;
}

static void CheckSwitches()
{
  if(digitalRead(PIN_SW_1)==LOW)
  {
    digitalWrite(PIN_UART_EN,HIGH);
    Serial.begin(19200);
    Serial.println("enter debug mode");
    EnterDebugMode();
    Serial.flush();
    Serial.end();
    digitalWrite(PIN_UART_EN,LOW);
  }
  
  if(digitalRead(PIN_SW_2)==LOW)
  {
    if(gForceUpload)  // avoid retransmitting
    {
      while(digitalRead(PIN_SW_2)==LOW)
      {
        delay(10);
      }
    }
    else
    {
      gForceUpload = true;
    }
  }
  else
  {
    gForceUpload = false;
  }
}

// ############################################################################################################################
// the setup function
void setup() 
{
  pinMode(PIN_SOL_CHARGE,OUTPUT);
  SetSState(M_OFF);
  
  pinMode(PIN_UART_EN, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_WLAN_EN, OUTPUT);
  
  digitalWrite(PIN_LED,LOW);
  digitalWrite(PIN_WLAN_EN,HIGH);
  digitalWrite(PIN_UART_EN,LOW);
  
  InitAnalogeSensors();

  pinMode(PIN_SW_1,INPUT_PULLUP);
  pinMode(PIN_SW_2,INPUT_PULLUP);
  
  analogReference(EXTERNAL);
  
  MeasureSensors();
  SignalLED(LED_INIT);
  Wire.begin();

  if(EEPROM_ENABLE)
  {
    if(!EEPROM_I2C_begin())
    {
      gHangUpFlag = true; // no EEPROM
    }
  }

  if(BME280_ENABLE)
  {
    if(!BME280_init())
    {
      gHangUpFlag = true; // no BME sensor
    }
  }
}

// the loop function
void loop() 
{
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  if(gHangUpFlag)
  {
    SignalLED(LED_ERROR);
  }
  else
  {
    MeasureSensors();
    WriteEeprom();
    PowerManagement();
    CheckSwitches();
    if(gPowerStatus == BAT_CHARGEING)
      SignalLED(LED_CHARGE_BLINK);
    DataUpload();
  }
}
