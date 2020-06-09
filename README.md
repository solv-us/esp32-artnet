# ESP32 Art-Net LED Matrix
### FastLED Driver + WiFiManager + OTA-updater 

A FastLED driver for the ESP32 that can be controlled through the Art-Net protocol over WiFi. Featuring a WiFi Portal to set up an internet connection, and an Over-The-Air update system. Ideal for use in places where there is no easy phsyical USB access to the ESP32.

![Flowchart](Flowchart.png)

Dependencies:

- https://github.com/FastLED/FastLED
- https://github.com/tzapu/WiFiManager
- https://github.com/rstephan/ArtnetWifi/
- https://github.com/espressif/arduino-esp32/tree/master/libraries/ArduinoOTA
