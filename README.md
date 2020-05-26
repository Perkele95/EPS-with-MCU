# EPS-with-MCU
This is software for ATtiny45 in an electrical power system for nanosatellite application. It is part of our groups bachelors
thesis for Orbit NTNU; Development of High Efficiency MPPT Solar Charger for CubeSats. The raw code for the main file is also 
present in the appendix of the thesis.

----------------
Main file:
eps_main.c
----------------
Only eps_main.c has been compiled in Atmel Studio 7.0.

eps_adc.c and eps_pwm.c are only rough drafts, and can be overlooked.


----------------
Preamble definitions:

REF_VCC 1.1 - the internal reference voltage of ATtiny45. It is more stable compared to VCC as Vref. It might be different on other AVR type MCUs.

VOLTAGE_DIV_FACTOR  9 - Division factor in the voltage divider. One could use two different divisions for input and output, as their voltage ceiling is different.

RSENSE 0.01 - Sense resistor value.

VBAT_LOW 5.6 - When the battery voltage drops below this value, the system assumes they need charging.

VCHARGE 7.2 - Charge voltage level. If power supply ORing is used, one needs to take into account the voltage drop across the diode or MOSFET.

IBAT_THRESHOLD 0.4375 - A ratio of the discharge rate current of the batteries. CCCV protocol ends when this threshold is reached.
