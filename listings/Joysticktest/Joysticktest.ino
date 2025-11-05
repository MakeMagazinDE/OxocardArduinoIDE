#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_ST77XX.h>
 
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC   27
#define TFT_RST  4
#define TFT_BL   19
 
#define JOY_UP     39
#define JOY_RIGHT  37
#define JOY_DOWN   38
#define JOY_LEFT   36
#define JOY_CENTER 2
 
#ifndef HSPI
#define HSPI 1
#endif
 
SPIClass hspi(HSPI);
Adafruit_ST7789 tft = Adafruit_ST7789(&hspi, TFT_CS, TFT_DC, TFT_RST);
 
bool last_up = false, last_down = false, last_left = false, last_right = false, last_center = false;
 
void drawJoystickState(bool up, bool down, bool left, bool right, bool center) {
  int y = 46;
  tft.fillRect(0, y, 240, 200, ST77XX_BLACK);
 
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
 
  tft.setCursor(10, y +  0);   tft.print("UP:     ");     tft.print(up     ? "X" : " ");
  tft.setCursor(10, y + 30);   tft.print("DOWN:   ");     tft.print(down   ? "X" : " ");
  tft.setCursor(10, y + 60);   tft.print("LEFT:   ");     tft.print(left   ? "X" : " ");
  tft.setCursor(10, y + 90);   tft.print("RIGHT:  ");     tft.print(right  ? "X" : " ");
  tft.setCursor(10, y + 120);  tft.print("CENTER: ");     tft.print(center ? "X" : " ");
}
 
void setup() {
  delay(300);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
 
  hspi.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  tft.init(240, 240);  // explizit Größe setzen!
  delay(100);
  tft.setRotation(3);
  tft.setSPISpeed(40000000);
  tft.fillScreen(ST77XX_BLACK);
 
  pinMode(JOY_UP, INPUT);
  pinMode(JOY_DOWN, INPUT);
  pinMode(JOY_LEFT, INPUT);
  pinMode(JOY_RIGHT, INPUT);
  pinMode(JOY_CENTER, INPUT);
}
 
bool firstDraw = true;
 
void loop() {
  bool up     = digitalRead(JOY_UP);
  bool down   = digitalRead(JOY_DOWN);
  bool left   = digitalRead(JOY_LEFT);
  bool right  = digitalRead(JOY_RIGHT);
  bool center = digitalRead(JOY_CENTER);
 
  if (firstDraw || up != last_up || down != last_down || left != last_left || right != last_right || center != last_center) {
    drawJoystickState(up, down, left, right, center);
    last_up = up;
    last_down = down;
    last_left = left;
    last_right = right;
    last_center = center;
    firstDraw = false;
  }
 
  delay(20);
}
