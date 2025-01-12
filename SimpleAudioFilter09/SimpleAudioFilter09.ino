/************************************************************* 
 * SimpleAudioFilter09.ino
 * Audio Filter DSP for Shorthwave Receiver
 * Based on Raspberry Pico Pi2 Processor module
 * 
 * Filter Functions :
 * 0. Audio passthrough 
 * 1..4 Filters ( SSB W / SSB N / CW w / CW N)
 * 
 * DNR NLMS Functions:
 * 0. Audio passthrough 
 * 1. DNR NLMS denoise Level 1 (low action)
 * 2. DNR NLMS denoise Level 2 (high action) 
 * 
 * With CW audio announce for mode and dnr change
 * 
 * Created: Start works on Feb 2023
 * Author: Giuseppe Callipo - IK8YFW
 * https://github.com/ik8yfw
 * 
 * License: Creative Common with attribution 
 * 
 * This program use filters built with the tFilter program
 * http://t-filter.engineerjs.com/
 * 
 * Last update: 03/01/2025
 * NOTE: This version does not works with Raspberry Pico Pi but
 *       need the new module Raspberry Pico Pi2 based on CortexM33 Architecture.
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
