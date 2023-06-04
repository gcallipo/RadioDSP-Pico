/************************************************************* 
 *  SimpleAudioFilter.ino
 * Audio Filter DSP for Shorthwave Receiver
 * Functions:
 * 1. Audio passthrough 
 * 2. SSB filter 3000 kHz + DNR low level
 * 3. CW  filter 800  Hz
 * 4. DNR High level
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
 */

#ifndef __AUDIOIO_H__
#define __AUDIOIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#define AVG_BIAS_SHIFT 1u
#define DAC_RANGE  256u
#define DAC_BIAS  (DAC_RANGE/2u)
#define ADC_RANGE  256u
#define ADC_BIAS  (ADC_RANGE/2u)

#define AVG_BIAS_SHIFT  1//8  //16 

void audioIO_setup(void);
void audioIO_loop(void);



#ifdef __cplusplus
}
#endif
#endif
