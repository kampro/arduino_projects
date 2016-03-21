#include "Adafruit_ILI9341.h"
#include "Sodaq_DS3231.h"

Adafruit_ILI9341 tft = Adafruit_ILI9341(10, 9);

const byte VOLUME_MIN = 0;
const byte VOLUME_MAX = 35;

enum EncoderPinAssignments {
  encoderPinA = 2, // rigth
  encoderPinB = 3, // left
};

volatile byte encoderPos = 0; // a counter for the dial
byte lastReportedPos = 1; // change management
boolean rotating = false; // debounce management

// interrupt service routine vars
boolean A_set = false;
boolean B_set = false;

byte oldSec, newSec;
byte oldDay, newDay;
float oldTemp, newTemp;


void setup() {
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  // turn on pullup resistors
  digitalWrite(encoderPinA, HIGH);
  digitalWrite(encoderPinB, HIGH);
  // encoder pin on interrupt 0
  attachInterrupt(0, doEncoderA, CHANGE);
  // encoder pin on interrupt 1
  attachInterrupt(1, doEncoderB, CHANGE);

  setupDisplay();

  Serial.begin(9600); // output
}

void loop() {
  rotating = true; // reset the debouncer

  if (lastReportedPos != encoderPos) {
    Serial.print("Index:");
    Serial.println(encoderPos, DEC);
    displayVolume(encoderPos);
    lastReportedPos = encoderPos;
  }

  newSec = rtc.now().second();
  if (newSec != oldSec) {
    displayTime(rtc.now());
  }
  oldSec = newSec;

  newDay = rtc.now().date();
  if (newDay != oldDay) {
    displayDate(rtc.now());
  }
  oldDay = newDay;

  newTemp = rtc.getTemperature();
  if (newTemp != oldTemp) {
    displayTemperature(newTemp);
  }
  oldTemp = newTemp;
}

// Interrupt on A changing state
void doEncoderA() {
  // debounce
  if (rotating) delay (1); // wait a little until the bouncing is done

  // Test transition, did things really change?
  if (digitalRead(encoderPinA) != A_set) { // debounce once more
    A_set = !A_set;

    // adjust counter + if A leads B
    if (A_set && !B_set) {
      if (encoderPos < VOLUME_MAX)
        ++encoderPos;
    }

    rotating = false; // no more debouncing until loop() hits again
  }
}

// Interrupt on B changing state
void doEncoderB() {
  if (rotating) delay (1);
  if (digitalRead(encoderPinB) != B_set) {
    B_set = !B_set;
    // adjust counter - 1 if B leads A
    if (B_set && !A_set) {
      if (encoderPos > VOLUME_MIN)
        --encoderPos;
    }

    rotating = false;
  }
}

void displayVolume(byte volume) {
  tft.setCursor(140, 140);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setTextSize(3);

  if (volume < 10) {
    tft.print(" ");
    tft.print(volume);
  }

  if (volume > 9)
    tft.print(volume);
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
  tft.setTextSize(2);
  tft.setTextColor(0xC618, ILI9341_BLACK);
  tft.print(temperature);
  tft.print("*C");
}

