// 0x28, 0x4C, 0x92, 0xC0, 0x06, 0x00, 0x00, 0xCA
// 0x28, 0x9B, 0xDB, 0xBF, 0x06, 0x00, 0x00, 0xA3
// 0x28, 0x71, 0x15, 0xC0, 0x06, 0x00, 0x00, 0xF7

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
char weekDay[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

const int UP         = 4;
const int DOWN       = 3;
const int LIMIT_LOW  = 0;
const int LIMIT_HIGH = 35;
int volume           = 0;
bool volumeUp        = LOW;
bool volumeDown      = LOW;
bool lastState       = LOW;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);
//DeviceAddress aThermometer, mThermometer, kThermometer;

void setup() 
{
  Serial.begin(9600); //57600
  Wire.begin();
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(0);
  rtc.begin();

  pinMode(UP, INPUT);
  pinMode(DOWN, INPUT);
  digitalWrite(UP, LOW);
  digitalWrite(DOWN, LOW);
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
}

uint32_t old_ts;

void loop() 
{
  DateTime now = rtc.now();
    uint32_t ts = now.getEpoch();
    if (old_ts == 0 || old_ts != ts) 
    {
     old_ts = ts;
     Serial.print(now.year(), DEC);
     Serial.print('/');
     Serial.print(now.month(), DEC);
     Serial.print('/');
     Serial.print(now.date(), DEC);
     Serial.print(' ');
     Serial.print(now.hour(), DEC);
     Serial.print(':');
     Serial.print(now.minute(), DEC);
     Serial.print(':');
     Serial.print(now.second(), DEC);
     Serial.print(' ');
     Serial.println(weekDay[now.dayOfWeek()]);
    }

  rtc.convertTemperature();

//GODZINA
  tft.setCursor(18, 25);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print("Godzina: ");
  if(now.hour() < 10)
        tft.print('0');
  tft.print(now.hour(), DEC);
  tft.print(':');
  if(now.minute() < 10)
        tft.print('0');
  tft.print(now.minute(), DEC);
  tft.print(':');
  if(now.second() < 10)
        tft.print('0');
  tft.print(now.second(), DEC);

//DATA
  tft.setCursor(18, 50);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print("Data:   ");
  if(now.date() < 10)
        tft.print('0');
  tft.print(now.date(), DEC);
  tft.print('-');
  if(now.month() < 10)
        tft.print('0');
  tft.print(now.month(), DEC);
  tft.print('-');
  if(now.year() < 10)
        tft.print('0');
  tft.print(now.year(), DEC);

//TEMPERATURA zegar
  tft.setCursor(0, 90);
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print("Temp");
  tft.setCursor(50, 95);
  tft.setTextSize(1);
  tft.print(" sterownika ");
  tft.setCursor(120, 90);
  tft.setTextSize(2);
  tft.print(": ");
  tft.setTextColor(0xC618, ILI9341_BLACK);
  tft.print(rtc.getTemperature());
  tft.print("*C");

//VOLUME
  noInterrupts();
  volumeUp   = digitalRead(UP);
  volumeDown = digitalRead(DOWN);
  interrupts();
  
  if (lastState == LOW)
  {
    if (volumeUp == HIGH && volume < LIMIT_HIGH)
      volume++;
    if (volumeDown == HIGH && volume > LIMIT_LOW)
      volume--;
  }

  if (volumeUp == HIGH || volumeDown == HIGH)
    lastState = HIGH;
  else
    lastState = LOW;

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
