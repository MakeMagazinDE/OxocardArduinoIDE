/*
 * File: Oxocard_ToF.ino
 *
 * Read the ToF Sensor VL53L5CX to measure distance up to 4 m.
 * 
 * 2025-07-27 Claus Kühnel info@ckuehnel.ch
 */

#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h>  // V.1.0.3
#include "SparkFun_External_EEPROM.h" // v.3.2.10
#include "pins.h"
#include "SPI.h"
#include "TFT_eSPI.h" // v2.5.43 Adaption to Oxocard Connect V2 required

// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

SparkFun_VL53L5CX mySensor;
VL53L5CX_ResultsData measurementData;
ExternalEEPROM myEEPROM;

int imageResolution = 0; //Used to pretty print output
int imageWidth = 0; //Used to pretty print output

void setup()
{
  pinMode(LCD_LED, OUTPUT); digitalWrite(LCD_LED, HIGH); // LCD background light on

  Serial.begin(115200);
  delay(2000);
  Serial.println("Oxocard Connect ToF Ranging");

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_GREEN);
  tft.setTextDatum(TC_DATUM);     // Centre text on x,y position
  tft.setFreeFont(&FreeMonoBold12pt7b);
  tft.drawString("Oxocard Connect", 120, 10);
  tft.drawString("ToF Ranging", 120,28);
  tft.setFreeFont(&FreeMonoBold18pt7b);

  Wire.begin();           //This resets to 100kHz I2C
  Wire.setClock(400000);  //Sensor has max I2C freq of 400kHz 

  Serial.println("Initializing sensor board. This can take up to 10s. Please wait.");
  if (mySensor.begin() == false)
  {
    Serial.println(F("Sensor not found - check your wiring. Freezing"));
    while (1) ;
  }
  
  mySensor.setResolution(4*4); //Enable 16 pads
  imageResolution = mySensor.getResolution(); //Query sensor for current resolution - either 4x4 or 8x8
  imageWidth = sqrt(imageResolution); //Calculate printing width
  Serial.printf("Resolution of VL53L5CX is %d\n", imageResolution);

  mySensor.startRanging();
}

void loop()
{
  //Poll sensor for new data
  if (mySensor.isDataReady() == true)
  {
    if (mySensor.getRangingData(&measurementData)) //Read distance data into array
    {
      //The ST library returns the data transposed from zone mapping shown in datasheet
      //Pretty-print data with increasing y, decreasing x to reflect reality
      for (int y = 0 ; y <= imageWidth * (imageWidth - 1) ; y += imageWidth)
      {
        for (int x = imageWidth - 1 ; x >= 0 ; x--)
        {
          Serial.print("\t");
          Serial.print(measurementData.distance_mm[x + y]);
        }
        Serial.println();
      }
      Serial.println();

      // Berechnung von Min, Max and Mean
      int minValue = 32767;
      int maxValue = 0;
      long sum = 0;

      for (int y = 0 ; y <= imageWidth * (imageWidth - 1) ; y += imageWidth)
      {
        for (int x = imageWidth - 1 ; x >= 0 ; x--)
        {
        int value = measurementData.distance_mm[x+y];
        sum += value;
        if (value < minValue) minValue = value;
        if (value > maxValue) maxValue = value;
        }
      }

      float average = (float) sum / imageResolution;
    
      // Print results
      Serial.println("\nResults:");
      Serial.printf("Mean = %6.1f\n", average);
      Serial.printf("Max  = %d\n", maxValue);
      Serial.printf("Min  = %d\n\n", minValue);

      if ((maxValue - minValue) > average/10 )
      {
        Serial.println("Attention - large deviation possible");
        tft.setTextColor(TFT_RED);
      }
      else tft.setTextColor(TFT_WHITE);

      tft.fillRect(0, 130, 240, 40, TFT_BLACK);
      tft.drawString(String(average) + " mm", 120, 140);
    }
  }
  delay(1000); //Small delay between polling
}
