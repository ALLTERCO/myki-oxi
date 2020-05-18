# MyKi Oxi

This project is based on [an example](http://www.esp32learning.com/code/esp32-and-max30100-heart-rate-monitor-sensor.php)
which dumps heart rate and SpO2 data on the serial port. We've added
support for the BLE Pulse Oximeter Service for ESP32.

For interpreting raw data from the sensor we use
[Arduino-MAX30100](https://github.com/oxullo/Arduino-MAX30100/).

Whatever other bits are here are in the public domain.

## Building

Install VSCode and the PlatformIO extension, get this source code,
open the project in PlatformIO, build, upload, enjoy.