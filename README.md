# RadioDSP-Pico
Audio Filter DSP for Shorthwave Receiver

This Is a Digital Signal Processing to compute the audio of Shortwave Radio receiver. The project Is based on the Raspberry Pico pi board. The project implements some real time filters, show how to setup a simple real time processing dsp chain using the ADC and the I2S in/out for audio. 

NEWS [18.02.2023] - The SimpleAudioFilter08 implements the dynamic filtering calculation, now it is possible to calculate on the fly 
the kernel fir coefficient based on windowed sinc alghoritm. It is simple to add user defined filter, now I defined 4 filter
SSB Wide (200-3400), SSB Narrow (300-1800), CW Wide (600 Hz) and CW Narrow (300 Hz).
All is work in progress.  

[11.02.2023] - The new version SimpleAudioFilter07 use the ADCInput and I2S services by earlephilhower/arduino-pico plugin. 
the output power amplifier use now a MAX98357A I2S 3W audio digital amplifier with good results.
The project implements 3 filters ( BPF 700 Hz , LPF 3000 Hz and LPF 6000 Hz ) and a simple Noise reduction at 3 levels.
It is possible to set the Power Aplifier Gain to drive a small headphone or a large speaker, use the PIN GPIO 11 ( set to GND for high gain speaker, floating for low gain headphones ), for safety reason the gain set will be read only at startup.
All is work in progress.

Here the new Videos:

CW    https://youtu.be/CpewdCyXoGU
SSB1  https://youtu.be/-HOMVBYqSmQ
SSB2  https://youtu.be/sMhqdDxAlq8

[28.01.2023] - I'm starting new experimental filter (SimpleAudioFilter06) using ADCInput and I2S services by earlephilhower/arduino-pico plugin. All is work in progress, but the firsts results are really good.

****************************************************************************************************
OLD VERSIONS.  
There are some old version available that implements similar processing but with some structural differences
and still available for educational pourposes
SimpleAudioFilter02,SimpleAudioFilter02, SimpleAudioFilter06 

NOTE: The project was build with Arduino IDE v. 1.8.19 with the earlephilhower/arduino-pico plugin.

11 Feb 2023 - Giuseppe Callipo IK8YFW

