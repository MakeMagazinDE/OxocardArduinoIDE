/*
  File: Oxocard_WiFi_Localization.ino
  This sketch demonstrates how to find the location of an Oxovard using the Google Geolocation API.
  (c) 2025-09-24 Claus Kühnel info@ckuehnel.ch

  Based on
  Localização por Wi-Fi com ESP32 usando a API Google Geolocation by Jean Mappelli
  https://embarcados.com.br/localizacao-por-wi-fi-com-esp32-usando-a-api-google-geolocation/
*/

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "pins.h"
#include "SPI.h"
#include "TFT_eSPI.h" // v2.5.43 Adaption to Oxocard Connect V2 required

// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

// WLAN Parameter
const char* ssid = "Sunrise_2.4GHz_19B4C2";
const char* password = "u2u7xxxxx1Ds";

// Replace with your Google API key
const char* apiKey = "AIzaSyAKkrrg5jKxxxxxxxb_WjFC2l48aBCA3Y";

void setup() 
{ 
    pinMode(LCD_LED, OUTPUT); digitalWrite(LCD_LED, HIGH); // LCD background light on

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_GREEN);
    tft.setTextDatum(TC_DATUM);     // Centre text on x,y position
    tft.setFreeFont(&FreeMonoBold12pt7b);
    tft.drawString("Oxocard Connect", 120, 10);
    tft.drawString("WiFi Localization", 120,28);
    //tft.setFreeFont(&FreeMonoBold18pt7b);
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE);

    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connect with WiFi");
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected!");
    Serial.print("IP-Address: ");
    String localIP = WiFi.localIP().toString();
    Serial.println(localIP);
    tft.drawString("IP: " + localIP, 0, 70);
}


void loop() 
{
  uint line = 120;
  tft.fillRect(0, line, 240, 240, TFT_BLACK);
  if (WiFi.status() == WL_CONNECTED) 
  {
    // Scans nearby Wi-Fi networks
    int n = WiFi.scanNetworks();
    Serial.printf("%d networks found\n", n);
    uint x = tft.drawNumber(n, 0, line);
    tft.drawString(" networks found", x, line);

    if (n > 0) 
    {
      // Build JSON to send to the API
      StaticJsonDocument<1024> doc;
      JsonArray wifiAccessPoints = doc.createNestedArray("wifiAccessPoints");

      for (int i = 0; i < n; i++) 
      {
        JsonObject ap = wifiAccessPoints.createNestedObject();
        ap["macAddress"] = WiFi.BSSIDstr(i);
        ap["signalStrength"] = WiFi.RSSI(i);
      }

      String requestBody;
      serializeJson(doc, requestBody);

      // Build API URL
      String url = "https://www.googleapis.com/geolocation/v1/geolocate?key=" + String(apiKey);

      HTTPClient http;
      http.begin(url);
      http.addHeader("Content-Type", "application/json");

      int httpResponseCode = http.POST(requestBody);

      if (httpResponseCode > 0) 
      {
        String response = http.getString();
        Serial.println("API response:");
        Serial.println(response);

        // Parse the response
        StaticJsonDocument<512> resp;
        DeserializationError error = deserializeJson(resp, response);

        if (!error) 
        {
          float lat = resp["location"]["lat"];
          float lng = resp["location"]["lng"];
          float acc = resp["accuracy"];
          Serial.printf("Latitude: %f\nLongitude: %f\nAccuracy: %.2f m\n\n", lat, lng, acc);

          tft.drawString("Lat:", 0, line+25); 
          tft.drawFloat(lat, 6, 80, line+25); 
          tft.drawString("Lon:", 0, line+50); 
          tft.drawFloat(lng, 6, 80, line+50); 
          tft.drawString("Acc:", 0, line+75); 
          x = tft.drawFloat(acc, 2, 80, line+75); 
          tft.drawString(" m", 80+x, line+75);
        }
      } else {
        Serial.printf("Error in request: %d\n", httpResponseCode);
      }
      http.end();
    }
  } else {
    tft.drawString("No WiFi connection", 0, line);
  }
  delay(30000); // Perform a new query every 30 s
}
