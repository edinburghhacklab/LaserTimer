#include "EEPROM.h"
#include "analogComp.h"
#include "LiquidCrystal.h"
#include <stdio.h>

unsigned long last_on;
unsigned long total;
boolean laser_on;

unsigned int value;

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

typedef struct eeprom {
  byte version;
  unsigned long total;
} eeprom_t;

eeprom_t eeprom;

char time_string[17];

#define LASER_THRESHOLD 10

void adcSetup(void) {
  /* Taken from Glen Sweeney's article at:
  http://www.glennsweeney.com/tutorials/interrupt-driven-analog-conversion-with-an-atmega328p
  */

  // clear ADLAR in ADMUX (0x7C) to right-adjust the result
  // ADCL will contain lower 8 bits, ADCH upper 2 (in last two bits)
  ADMUX &= B11011111;

  // Set REFS1..0 in ADMUX (0x7C) to change reference voltage to the
  // proper source (01)
  ADMUX |= B01000000;

  // Clear MUX3..0 in ADMUX (0x7C) to select analog channel 0.
  ADMUX &= B11110000;

  // Set ADEN in ADCSRA (0x7A) to enable the ADC.
  // Note, this instruction takes 12 ADC clocks to execute
  ADCSRA |= B10000000;

  // Set ADATE in ADCSRA (0x7A) to enable auto-triggering.
  ADCSRA |= B00100000;

  // Clear ADTS2..0 in ADCSRB (0x7B) to set trigger mode to free running.
  // This means that as soon as an ADC has finished, the next will be
  // immediately started.
  ADCSRB &= B11111000;

  // Set the Prescaler to 128 (16000KHz/128 = 125KHz)
  // Above 200KHz 10-bit results are not reliable.
  ADCSRA |= B00000111;

  // Set ADIE in ADCSRA (0x7A) to enable the ADC interrupt.
  // Without this, the internal interrupt will not trigger.
  ADCSRA |= B00001000;

  // Enable global interrupts
  // AVR macro included in <avr/interrupts.h>, which the Arduino IDE
  // supplies by default.
  sei();

  // Set ADSC in ADCSRA (0x7A) to start the ADC conversion
  ADCSRA |=B01000000;
}

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
  laser_on = analogRead(0) > LASER_THRESHOLD;
  analogComparator.setOn(AIN0, AIN1);
  analogComparator.enableInterrupt(supplyFalling, FALLING);
  lcd.begin(16, 2);
  lcd.print("Laser Tube Hours");
  lcd.setCursor(0, 1);
  format(total);
  lcd.print(time_string);
  adcSetup();
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

ISR(ADC_vect) {
  unsigned int value = ADCL | (ADCH << 8);
  unsigned long time = millis();
  boolean now_on = value > LASER_THRESHOLD;
  if (laser_on && !now_on)
    total += time - last_on;
  else if (now_on && !laser_on)
    last_on = time;
  laser_on = now_on;
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
