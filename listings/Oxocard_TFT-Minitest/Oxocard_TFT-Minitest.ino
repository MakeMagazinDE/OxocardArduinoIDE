#include <TFT_eSPI.h>

// Oxocard Connect V2 Mini TFT Test

#include "pins.h"
#include "SPI.h"
#include "TFT_eSPI.h" // v2.5.43 Adaption to Oxocard Connect V2 required

// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

void setup() 
{
  pinMode(LCD_LED, OUTPUT); 
  digitalWrite(LCD_LED, HIGH); // LCD background light on

  Serial.begin(115200);
  delay(1000);
  Serial.println("Oxocard Connect V2 Mini TFT Test");
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_GREEN);
  tft.setTextDatum(TC_DATUM);     // Centre text on x,y position
  tft.setFreeFont(&FreeMonoBold12pt7b);
  tft.drawString("Oxocard Connect", 120, 10);
  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(&FreeMono12pt7b);
  tft.drawString("Mini TFT Test", 120, 50);
  tft.drawString("If you see this,", 120, 70);
  tft.drawString("TFT works!", 120, 90);

  Serial.println("TFT initialization and text drawing complete.");
}

void loop() 
{
  // Empty loop for mini test
  delay(1000); // Just to keep it running without doing anything
}
