# RadioDSP-Pico
Audio Filter DSP for Shorthwave Receiver

This Is a Digital Signal Processing to compute the audio of Shortwave Radio receiver. The project Is based on the Raspberry Pico pi board. The project implements some real time filters, show how to setup a simple real time processing dsp chain using the ADC and the PWM in/out for audio. All the works are in progress. 

NEWS [28.01.2023] - I'm starting new experimental filter (SimpleAudioFilter06) using ADCInput and I2S services by earlephilhower/arduino-pico plugin. All is work in progress, but the firsts results are really good.

****************************************************************************************************
OLD VERSIONS.  
There are some version available that implements similar processing but with some structural differences
:

[02.01.2023]
 New Version: SimpleAudioFilter02 use the full 12 bit ADC and PWM output
 Functions:
 1. Audio passthrough 
 2. DNR Average High level denoise 
 3. DNR + DNF Average Denoise + LMS Filter (NB & Beat eliminator)
 4. AM/SSB filter 6000 kHz + DNR low level
 5. SSB filter    3000 kHz + DNR medium level
 6. CW  filter    700  Hz  + DNR low level


[08.12.2022] 
  Older version: SimpleAudioFilter01 use a basic 8 bit ADC resolution and PWM output.
  Functions (Accessible by push button):
  1. Audio passthrough (on startup)
  2. DNR High level
  3. AM/SSB filter 6000 kHz + DNR low level
  5. SSB filter 3000 kHz + DNR medium level
  5. CW  filter 800  Hz  + DNR low level

NOTE: The project was build with Arduino IDE v. 1.8.19 with the Arduino MBED OS RP2040 v.3.3.0 installed.

    

02 Jan 2023 - Giuseppe Callipo IK8YFW

