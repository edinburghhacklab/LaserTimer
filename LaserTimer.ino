#include "EEPROM.h"
#include "analogComp.h"

unsigned long last_on;
unsigned long total;
boolean laser_on;

void setup() {
  unsigned int i;
  for (i = 0; i < sizeof(total); i++)
    *(((byte *) &total) + i) = EEPROM.read(i);
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
  unsigned int i;
  for (i = 0; i < sizeof(total); i++)
    EEPROM.write(i, *(((byte *) &total) + i));
  while (1);
}
