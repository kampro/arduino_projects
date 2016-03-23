#include "Adafruit_ILI9341.h"
#include "Sodaq_DS3231.h"
#include <DallasTemperature.h>
#include <OneWire.h>

enum EncoderPinAssignments {
  encoderPinA = 2, // rigth
  encoderPinB = 3, // left
};

enum ThermometerPinAssigments {
  thermometerPin = 4
};

enum VoltagePinAssigments {
  voltagePin = A2
};

Adafruit_ILI9341 tft = Adafruit_ILI9341(10, 9);
OneWire oneWire(thermometerPin);
DallasTemperature sensors(&oneWire);

const byte VOLUME_MIN = 0;
const byte VOLUME_MAX = 35;

volatile int encoderPos = 0;
int lastReportedPos = 1;
boolean busy = false;
boolean shouldContinue = false;

// interrupt service routine vars
boolean A_set = false;
boolean B_set = false;

byte oldSec, newSec;
byte oldDay, newDay;
float oldDriverTemp, newDriverTemp;
float oldATemp, newATemp;
float oldM50Temp, newM50Temp;
float oldK24Temp, newK24Temp;

float oldVoltage, newVoltage;

void setup() {
  Serial.begin(9600); // output

  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  // turn on pullup resistors
  digitalWrite(encoderPinA, HIGH);
  digitalWrite(encoderPinB, HIGH);
  // encoder pin on interrupt 0
  attachInterrupt(0, doEncoderA, CHANGE);
  // encoder pin on interrupt 1
  attachInterrupt(1, doEncoderB, CHANGE);

  sensors.begin();

  setupDisplay();
}

void loop() {
  //rotating = true; // reset the debouncer

  //  if (lastReportedPos != encoderPos) {
  //    displayVolume(encoderPos);
  //    lastReportedPos = encoderPos;
  //  }

  newSec = rtc.now().second();
  if (newSec != oldSec) {
    displayTime(rtc.now());
    oldSec = newSec;
  }

  newDay = rtc.now().date();
  if (newDay != oldDay) {
    displayDate(rtc.now());
    oldDay = newDay;
  }

  newDriverTemp = rtc.getTemperature();
  if (newDriverTemp != oldDriverTemp) {
    displayTemperature(newDriverTemp);
    oldDriverTemp = newDriverTemp;
  }

  sensors.requestTemperatures();

  newATemp = sensors.getTempCByIndex(0);
  if (newATemp != oldATemp) {
    displayATemperature(newATemp);
    oldATemp = newATemp;
  }

  newM50Temp = sensors.getTempCByIndex(1);
  if (newM50Temp != oldM50Temp) {
    displayM50Temperature(newM50Temp);
    oldM50Temp = newM50Temp;
  }

  newK24Temp = sensors.getTempCByIndex(2);
  if (newK24Temp != oldK24Temp) {
    displayK24Temperature(newK24Temp);
    oldK24Temp = newK24Temp;
  }

  newVoltage = (float) analogRead(voltagePin) / 1024 * 5;
  if (newVoltage != oldVoltage) {
    displayVoltage(newVoltage);
    oldVoltage = newVoltage;
  }
}

// Interrupt on A changing state
void doEncoderA() {
  shouldContinue = true;
  if (busy == false) {
    busy = true;
    if (digitalRead(encoderPinA) != A_set) {
      A_set = !A_set;

      // adjust counter + if A leads B
      if (A_set && !B_set) {
        if (encoderPos < VOLUME_MAX)
          ++encoderPos;
      }

      displayVolume(encoderPos);
      Serial.println(encoderPos);

      //Serial.println(encoderPos);
    }
    busy = false;
  }

}

// Interrupt on B changing state
void doEncoderB() {
  shouldContinue = true;
  if (busy == false) {
    busy = true;
    if (digitalRead(encoderPinB) != B_set) {
      B_set = !B_set;
      // adjust counter - 1 if B leads A
      if (B_set && !A_set) {
        if (encoderPos > VOLUME_MIN)
          --encoderPos;
      }

      displayVolume(encoderPos);
      Serial.println(encoderPos);

    }
    busy = false;
  }
}

void setupDisplay() {
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);

  tft.setCursor(0, 90);
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print("Driver temp:");
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

void displayDate(DateTime timestamp) {
  tft.setCursor(118, 50);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setTextSize(2);
  if (timestamp.date() < 10)
    tft.print('0');
  tft.print(timestamp.date(), DEC);
  tft.print('-');
  if (timestamp.month() < 10)
    tft.print('0');
  tft.print(timestamp.month(), DEC);
  tft.print('-');
  if (timestamp.year() < 10)
    tft.print('0');
  tft.print(timestamp.year(), DEC);
}

void displayTime(DateTime timestamp) {
  tft.setCursor(142, 25);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setTextSize(2);
  if (timestamp.hour() < 10)
    tft.print('0');
  tft.print(timestamp.hour(), DEC);
  tft.print(':');
  if (timestamp.minute() < 10)
    tft.print('0');
  tft.print(timestamp.minute(), DEC);
  tft.print(':');
  if (timestamp.second() < 10)
    tft.print('0');
  tft.print(timestamp.second(), DEC);
}

void displayTemperature(float temperature) {
  tft.setCursor(150, 90);
  tft.setTextColor(0xC618, ILI9341_BLACK);
  tft.setTextSize(2);  
  tft.print(temperature);
  tft.print("*C");
}

void displayVolume(int volume) {
  tft.setCursor(140, 140);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setTextSize(3);

  if (volume < 10) {
    tft.print(" ");
    tft.print(volume, DEC);
  }

  if (volume > 9)
    tft.print(volume, DEC);
}

void displayATemperature(float temperature) {
  tft.setCursor(175, 200);
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print(temperature);
}

void displayM50Temperature(float temperature) {
  tft.setCursor(175, 230);
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print(temperature);
}

void displayK24Temperature(float temperature) {
  tft.setCursor(175, 260);
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print(temperature);
}

void displayVoltage(float voltage) {
  tft.setCursor(175, 290);
  tft.setTextColor(ILI9341_ORANGE, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print(voltage);
}

