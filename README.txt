Laser Cutter Timer
------------------

Copyright (c) 2013 Martin Ling & Peter Jackson

Edinburgh Hacklab
http://www.edinburghhacklab.com

Features:

- Tracks total laser tube usage time.
- Writes total to EEPROM when power is turned off.

Requires:

- Arduino Uno
- Analog Comparator library by Leonardo Miliani:
  http://www.leonardomiliani.com/2012/analogcomp-una-libreria-per-gestire-il-comparatore-analogico/?lang=en
- External circuitry as shown below.

Wiring diagram:

D2 -- Laser PSU Enable Input

5V --+
     |
    | | 5.6k
    | |
     |
D7 --+------+
     |      |
    | | 10k | 1uF
    | |    ===
     |      |
GND -+------+

24V -+
     |
    | | 560k
    | |
     |
D6 --+-------+
     |       |
    | | 100k | 470uF
    | |     ===
     |       |
GND -+-------+
