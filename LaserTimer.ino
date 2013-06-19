#include "EEPROM.h"
#include "analogComp.h"

unsigned long last_on;
unsigned long total;
boolean laser_on;

void setup() {
  readEEPROM(0, &total, sizeof(total));
  Serial.begin(115200);
  laser_on = digitalRead(2);
  attachInterrupt(0, inputChanged, CHANGE);
  analogComparator.setOn(AIN0, AIN1);
  analogComparator.enableInterrupt(supplyFalling, FALLING);
}

void loop() {
  if (laser_on)
    Serial.println(total + millis() - last_on);
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
  writeEEPROM(0, &total, sizeof(total));
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
