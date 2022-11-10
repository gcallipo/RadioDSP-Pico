
/*
 * SimpleAudioFilter.ino
 * 
 * Created: Nov 2022
 * Author: Giuseppe Callipo - IK8YFW
 * https://github.com/ik8yfw
 * 
 * 
*/
#ifndef SSB2FILTER_H_
#define SSB2FILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 16000 Hz

fixed point precision: 8 bits

* 0 Hz - 3500 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = n/a

* 4000 Hz - 8000 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = n/a

*/

#define SSB2FILTER_TAP_NUM 33

typedef struct {
  int history[SSB2FILTER_TAP_NUM];
  unsigned int last_index;
} SSB2Filter;

void SSB2Filter_init(SSB2Filter* f);
void SSB2Filter_put(SSB2Filter* f, int input);
int SSB2Filter_get(SSB2Filter* f);

#endif
