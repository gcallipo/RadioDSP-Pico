# RadioDSP-Pico
Audio Filter DSP for Shorthwave Receiver

This Is a Digital Signal Processing to compute the audio of Shortwave Radio receiver. The project Is based on the Raspberry Pico pi board. The project implements some real time filters, show how to setup a simple real time processing dsp chain using the ADC and the I2S in/out for audio. 
Two versions are available at the moment:  SimpleAudioFilter08 works with Pico Pi1 and Pico Pi2 processors, SimpeAudioFilter09 works only with Pico Pi2.

NEWS [12.01.2025] - RELEASE SimpleAudioFilter09 : a new version of the firmware that implements Digital Noise Reduction LMS with good results. 
ATTENTION: The new firmware works with the same HW circuit but requires a RaspBerry Pico Pi 2 module without other circuit modifications,
therefore, if you have built the filter, and want to use this new version you will have to change only the Raspberry Pico module 
with the Pico 2 and load the SimpleAudioFilter09 Firmware.

Question and Answer:

Q: Does SimpleAudioFilter09 firmware work with the Raspberry Pico Pi 1?

A: No, a Rasberry Pico Pi 2 module is required.


Q: If I'm not interested in Noise Reduction, is it necessary to upgrade?

A: No, because the filters remain unchanged.


Q: What is the difference between the old Noise Reduction (SimpleAudioFiler08) and the new one (SimpleAudioFiler09) ?

A: The previous Noise reduction is very simple and is based on a Median Filter which 
   performs an arithmetic average on the acquired samples to smooth out any peaks 
   on the acquired signal and the audio effect is modest.
   The new Noise Reduction is based on a Least Mean Square algorithm which is a particular one 
   adaptive filter which, with the continuous calculation of coefficients, manages to reduce the 
   input noise compared to the useful signal, the effect on the acquired signal is great 
   significant. It is one of the most used algorithms on commercial transceivers.


Some video that show in action the new Noise Reduction, the Filter was connected to an FT 817 and driven an external speaker:

Video 1: https://youtu.be/EvLYVzMn99M?si=gbbvi2po9-flEtfQ

Video 2:

----------------------------------------------------------------------------------------------
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

He test the filter with an ICOM ic7300,  we can listen the various filter settings.

****************************************************************************************************
OLD VERSIONS.  
There are some old version available that implements similar processing but with some structural differences
and still available for educational pourposes
SimpleAudioFilter02,SimpleAudioFilter02, SimpleAudioFilter06, SimpleAudioFiler07 

Note: starting from the (SimpleAudioFilter06) we use the ADCInput and I2S services by earlephilhower/arduino-pico plugin. 

All is work in progress, but the firsts results are really good.
****************************************************************************************************


NOTE: The project was build with Arduino IDE v. 1.8.19 and v. 2.x.x with the earlephilhower/arduino-pico plugin.
      The pre-compiled versione is related to the latest version SimpleAudioFilter8.
      
12 Jan 2025 - Giuseppe Callipo IK8YFW

