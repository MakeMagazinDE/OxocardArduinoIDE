/*
  File: Oxocard_Cartridge_Detection.ino
  
  This sketch demonstrates how to detect a Oxocard Cartridge.
  
  (c) 2025-08-15 Claus Kühnel info@ckuehnel.ch
*/

#include <Wire.h>
#include "SparkFun_External_EEPROM.h" // Click here to get the library: http://librarymanager/All#SparkFun_External_EEPROM

ExternalEEPROM myEEPROM;

void setup()
{
  Serial.begin(115200);
  delay(2000);
  
  Wire.begin();
}

void loop()
{
  Serial.println("\nOxocard Connect Cartridge Detection");
  if (myEEPROM.begin() == false) Serial.println("No cartridge detected.");
  else 
  {
    Serial.println("Cartridge detected.");

    for (uint16_t x = 14; x < 32 * 4; x++)
    {
      char c = myEEPROM.read(x);
      if (c == 0) break;
      else Serial.print(c);
    }
    Serial.println(" Cartridge connected.");
  }
  delay(5000);
}