# Capacitor tester project
CS207 Winter 2018
This is project to develop code and a circuit to use an Arduino to test the capacitance of an unknown capacitor. 

The general method to do this will be to charge a capacitor to a known voltage and time it's discharge through a resistor of a known value. Using the arduino we should be able to measure the time it takes for any capacitor to discharge to a certain percentage of it's initial value. This will allow us to compute Tau in software. Finding Tau will allow us to compute the capacitance. (https://en.wikipedia.org/wiki/RC_time_constant). 


It may be possible to extend this circuit to measure leakage and ESR but this will be out of the scope of the project at this point. This would create a well rounded capacitor tester without the need for expensive lab equipment.

The schematic to build this project is included as schematic1.jpg and schematic2.jpg, however resistor values and part numbers are omitted at this time. The Arduino source code is included as capsketch.ino and builds with Arduino 1.8.5.
