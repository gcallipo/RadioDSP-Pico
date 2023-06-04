
/************************************************************* 
 *  SimpleAudioFilter02.ino
 * Audio Filter DSP for Shorthwave Receiver
 * Functions:
 * 0. Audio passthrough 
 * 1. DNR Average denoise 
 * 2. DNR + DNF Average Denoise + LMS Filter (NB & Beat eliminator)
 * 3. AM/SSB filter 6000 kHz + DNR low level
 * 4. SSB filter    3000 kHz + DNR medium level
 * 5. CW  filter    700  Hz  + DNR low level
 * 
 * 
 * 
 * Created: Nov 2022
 * Author: Giuseppe Callipo - IK8YFW
 * https://github.com/ik8yfw
 * 
 * License: Creative Common with attribution 
 * 
 * This program use filters built with the tFilter program
 * http://t-filter.engineerjs.com/
 * 
 * Last update: 02/01/2022
 */
#ifndef __AUDIOIO_H__
#define __AUDIOIO_H__

#ifdef __cplusplus
extern "C" {
#endif
 
//#define DEBUG_SERIAL

#define AVG_BIAS_SHIFT 1u
#define DAC_RANGE  256u
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
