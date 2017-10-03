# SP_LED
Code for running the ligthts in the Svartpuben
---
##Hardware
The hardware is based on the arduino platform running a strip of WS2812 leds. The arduino used is a Arduino nano with a CH340 USB to serial chip. Make sure that the correct drivers are installed before trying to program the Arduino,

Connected to the arduino is a MSGEQ7 used to divide the sound picked up by the circuit into different frequency bands. This data is used to drive animations in sync with the sound comming out from the speakers.

---
##Software
The software is written in the Arduino variant of C and the FastLED library found [here](fastled.io) is used to interface with the led-strip
