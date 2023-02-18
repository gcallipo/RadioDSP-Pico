
/************************************************************* 
 * SimpleAudioFilter07.ino
 * Audio Filter DSP for Shorthwave Receiver
 * 
 * Filter Functions :
 * 0. Audio passthrough 
 * 1. AM/SSB filter 6000 kHz 
 * 2. SSB filter    3000 kHz
 * 3. CW  filter    700  Hz
 * 
 * DNR Functions:
 * 0. Audio passthrough 
 * 1. DNR Average denoise Level 1
 * 2. DNR Average denoise Level 2
 * 3. DNR Average denoise Level 3
 * 
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
 * Last update: 11/02/2023
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
