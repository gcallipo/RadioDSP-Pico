# RadioDSP-Pico
Audio Filter DSP for Shorthwave Receiver

This Is a Digital Signal Processing to compute the audio of Shortwave Radio receiver. The project Is based on the Raspberry Pico pi board. The project implements some real time filters, show how to setup a simple real time processing dsp chain using the ADC and the I2S in/out for audio. 

FIX [24.11.2023] - Change the setI2s by reference, to fix bug when compile on Arduino 2.x and plugin 3.6.0. Thanks David G4CJF for reporting the issue and solution.

NEWS [13.08.2023] - Added new schematic board. For filter, the mono switch must be set to MONO and only one input autio source must be apply.

NEWS [07.07.2023] - The SimpleAudioFilter08 implements the dynamic filtering calculation, now it is possible to calculate on the fly 
the kernel fir coefficient based on windowed sinc alghoritm. It is simple to add user defined filter, now I defined 4 filter
SSB Wide (200-3400), SSB Narrow (300-1800), CW Wide (600 Hz) and CW Narrow (300 Hz). The DNR with simple average filter works with two levels, and a cw announce was added to indicate the filter mode and dnr mode selected. 
To avoid RFI interference I changed the output DAC with the PCM5102A, this dac is missed of internal Power amplifier, but it is noise free and
enough for eadphone use. You can add a simple additional lm386 or similar power amplifier if you need mmore loud audio, but avoid cheap Class D amplifier when use the filter in both with an RTX. 
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

[30.04.2024] Other users Videos:
Thanks to Antonio  Tafuto IW0FCI (ex. IW8ASM) for the video of his great built:

 https://www.youtube.com/watch?v=aX2tm2u037I 

He test the filter with and ICOM ic7300,  we can listen the various filter settings.

****************************************************************************************************
OLD VERSIONS.  
There are some old version available that implements similar processing but with some structural differences
and still available for educational pourposes
SimpleAudioFilter02,SimpleAudioFilter02, SimpleAudioFilter06 

Note: starting from the (SimpleAudioFilter06) we use the ADCInput and I2S services by earlephilhower/arduino-pico plugin. All is work in progress, but the firsts results are really good.

NOTE: The project was build with Arduino IDE v. 1.8.19 with the earlephilhower/arduino-pico plugin.

11 Feb 2023 - Giuseppe Callipo IK8YFW

