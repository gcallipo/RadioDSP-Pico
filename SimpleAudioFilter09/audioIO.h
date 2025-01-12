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
#ifndef __AUDIOIO_H__
#define __AUDIOIO_H__

#ifdef __cplusplus
extern "C" {
#endif
 
#define DEBUG_SERIAL

#define AVG_BIAS_SHIFT 1u
#define DAC_RANGE  4096u
#define DAC_BIAS  (DAC_RANGE/2u)
#define ADC_RANGE  4096u
#define ADC_BIAS  (ADC_RANGE/2u)

#define ADC_BIAS_SHIFT  20 

void audioIO_setup(void);
void audioIO_loop(void);

#ifdef __cplusplus
}
#endif
#endif
