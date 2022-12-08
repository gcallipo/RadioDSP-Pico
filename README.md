# RadioDSP-Pico
Audio Filter DSP for Shorthwave Receiver

This Is a Digital Signal Processing to compute the audio of Shortwave Radio receiver. The project Is based on the Raspberry Pico pi board. The project implements some real time filters, show how to setup a simple real time processing dsp chain using the ADC and the PWM in/out for audio. All the works are in progress. 

[08.12.2022] updated priject and improved the filters
  Functions (Accessible by push button):
  1. Audio passthrough (on startup)
  2. DNR High level
  3. AM/SSB filter 6000 kHz + DNR low level
  5. SSB filter 3000 kHz + DNR medium level
  5. CW  filter 800  Hz  + DNR low level

NOTE: The project was build with Arduino IDE v. 1.8.19 with the Arduino MBED OS RP2040 v.3.3.0 installed.

Video of SSB test (very first tests, must be updated):  https://www.youtube.com/watch?v=feivPPi68Uc
Video of CW  test (very first tests, must be updated):  https://www.youtube.com/watch?v=AmVc0JDxThY
    

08 December 2022 - Giuseppe Callipo IK8YFW

