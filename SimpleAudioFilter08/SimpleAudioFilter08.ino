/************************************************************* 
 * SimpleAudioFilter08.ino
 * Audio Filter DSP for Shorthwave Receiver
 * 
 * Filter Functions :
 * 0. Audio passthrough 
 * 1..4 Filters ( SSB W / SSB N / CW w / CW N)
 * 
 * DNR Functions:
 * 0. Audio passthrough 
 * 1. DNR Average denoise Level 1
 * 2. DNR Average denoise Level 2
 * 
 * With CW audio announce for mode and dnr change
 * 
 * Created: Feb 2023
 * Author: Giuseppe Callipo - IK8YFW
 * https://github.com/ik8yfw
 * 
 * License: Creative Common with attribution 
 * 
 * This program use filters built with the tFilter program
 * http://t-filter.engineerjs.com/
 * 
 * Last update: 07/07/2023
 */

 

#include "audioIO.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void setup() {

#ifdef DEBUG_SERIAL 
  Serial.begin(115200);
#endif  

  // Keep SMPS PWM mode to reduce audio noise. (RPi Pico Dataseet p.18)
  pinMode(23, OUTPUT);
  digitalWrite(23, HIGH);

  audioIO_setup();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void loop(void)
{
  audioIO_loop();
}
