#include "EEPROM.h"
#include "analogComp.h"
#include "LiquidCrystal.h"
#include <stdio.h>

unsigned long last_on;
unsigned long total;
boolean laser_on;

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

typedef struct eeprom {
  byte version;
  unsigned long total;
} eeprom_t;

eeprom_t eeprom;

char time_string[17];

void format(unsigned long milliseconds) {
  unsigned int hours = milliseconds / 3600000;
  milliseconds -= 3600000 * hours;
  unsigned int minutes = milliseconds / 60000;
  milliseconds -= 60000 * minutes;
  unsigned int seconds = milliseconds / 1000;
  milliseconds -= 1000 * seconds;
  sprintf(time_string, " %4dh%02dm%02d.%03ds", hours, minutes, seconds, milliseconds);
}

void setup() {
  readEEPROM(0, &eeprom, sizeof(eeprom));
  if (eeprom.version != 0) {
    eeprom.version = 0;
    eeprom.total = 0;
  }
  total = eeprom.total;
  Serial.begin(115200);
  laser_on = digitalRead(2);
  attachInterrupt(0, inputChanged, CHANGE);
  analogComparator.setOn(AIN0, AIN1);
  analogComparator.enableInterrupt(supplyFalling, FALLING);
  lcd.begin(16, 2);
  lcd.print("Laser Tube Hours");
  lcd.setCursor(0, 1);
  format(total);
  lcd.print(time_string);
}

void loop() {
  if (laser_on) {
    unsigned long time = total + millis() - last_on;
    format(time);
    Serial.println(time_string);
    lcd.setCursor(0, 1);
    lcd.print(time_string);
  }
}

void inputChanged() {
  unsigned long time = millis();
  if (laser_on)
    total += time - last_on;
  else
    last_on = time;
  laser_on = digitalRead(2);
}

void supplyFalling() {
  eeprom.total = total;
  writeEEPROM(0, &eeprom, sizeof(eeprom));
  while (1);
}

void readEEPROM(unsigned int src, void *dest, unsigned int size) {
  unsigned int i;
  for (i = 0; i < size; i++)
    *(((byte *) dest) + i) = EEPROM.read(src + i);
}

void writeEEPROM(unsigned int dest, void *src, unsigned int size) {
  unsigned int i;
  for (i = 0; i < size; i++)
    EEPROM.write(dest + i, *(((byte *) src) + i));
}
