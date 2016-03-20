#include <Wire.h>
#include <SPI.h>
#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include "Sodaq_DS3231.h"
#include <DallasTemperature.h>
#include <OneWire.h>

#define TFT_DC 9
#define TFT_CS 10
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9
char weekDay[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

const byte LIMIT_LOW  = 0;
const byte LIMIT_HIGH = 35;
byte volume           = 0;

#define encoder0PinA 3
#define encoder0PinB 4
volatile unsigned int encoder0Pos = 0;
unsigned int tmp                  = 0;
unsigned int Aold                 = 0;
unsigned int Bnew                 = 0;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void setup() {
  Wire.begin();

  rtc.begin();

  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(0);
  tft.setCursor(0, 140);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setTextSize(3);
  tft.print("Volume:");
  tft.setCursor(0, 200);
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print("STEG Classe A:");
  tft.setCursor(0, 230);
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print("Sony XES M50:");
  tft.setCursor(0, 260);
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print("STEG K24:");
  tft.setCursor(0, 290);
  tft.setTextColor(ILI9341_ORANGE, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print("Voltage:");

  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);
  // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(0, doEncoderA, CHANGE);
  // encoder pin on interrupt 1 (pin 3)
  attachInterrupt(1, doEncoderB, CHANGE);

  //  digitalWrite(UP, LOW);
  //  digitalWrite(DOWN, LOW);
}

uint32_t old_ts;

void loop() {
  //  DateTime now = rtc.now();
  //  uint32_t ts = now.getEpoch();
  //  if (old_ts == 0 || old_ts != ts)
  //  {
  //    old_ts = ts;
  //    Serial.print(now.year(), DEC);
  //    Serial.print('/');
  //    Serial.print(now.month(), DEC);
  //    Serial.print('/');
  //    Serial.print(now.date(), DEC);
  //    Serial.print(' ');
  //    Serial.print(now.hour(), DEC);
  //    Serial.print(':');
  //    Serial.print(now.minute(), DEC);
  //    Serial.print(':');
  //    Serial.print(now.second(), DEC);
  //    Serial.print(' ');
  //    Serial.println(weekDay[now.dayOfWeek()]);
  //  }
  //
  //  rtc.convertTemperature();
  //
  //  //GODZINA
  //  tft.setCursor(18, 25);
  //  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  //  tft.setTextSize(2);
  //  tft.print("Godzina: ");
  //  if (now.hour() < 10)
  //    tft.print('0');
  //  tft.print(now.hour(), DEC);
  //  tft.print(':');
  //  if (now.minute() < 10)
  //    tft.print('0');
  //  tft.print(now.minute(), DEC);
  //  tft.print(':');
  //  if (now.second() < 10)
  //    tft.print('0');
  //  tft.print(now.second(), DEC);
  //
  //  //DATA
  //  tft.setCursor(18, 50);
  //  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  //  tft.setTextSize(2);
  //  tft.print("Data:   ");
  //  if (now.date() < 10)
  //    tft.print('0');
  //  tft.print(now.date(), DEC);
  //  tft.print('-');
  //  if (now.month() < 10)
  //    tft.print('0');
  //  tft.print(now.month(), DEC);
  //  tft.print('-');
  //  if (now.year() < 10)
  //    tft.print('0');
  //  tft.print(now.year(), DEC);
  //
  //  //TEMPERATURA zegar
  //  tft.setCursor(0, 90);
  //  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  //  tft.setTextSize(2);
  //  tft.print("Temp");
  //  tft.setCursor(50, 95);
  //  tft.setTextSize(1);
  //  tft.print(" sterownika ");
  //  tft.setCursor(120, 90);
  //  tft.setTextSize(2);
  //  tft.print(": ");
  //  tft.setTextColor(0xC618, ILI9341_BLACK);
  //  tft.print(rtc.getTemperature());
  //  tft.print("*C");
}

// Interrupt on A changing state
void doEncoderA() {
  Bnew^Aold ? encoder0Pos++ : encoder0Pos--;
  Aold = digitalRead(encoder0PinA);
  displayVolume(encoder0Pos);
}
// Interrupt on B changing state
void doEncoderB() {
  Bnew = digitalRead(encoder0PinB);
  Bnew^Aold ? encoder0Pos++ : encoder0Pos--;
  displayVolume(encoder0Pos);
}

void displayVolume(byte volume) {
  tft.setCursor(140, 140);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setTextSize(3);

  if (volume == 0)
    tft.print("--");

  if (volume < 10 && volume > 0)
  {
    tft.print("0");
    tft.print(volume);
  }

  if (volume >= 10)
    tft.print(volume);
}
