
/*
 * SimpleAudioFilter.ino
 * 
 * Created: Nov 2022
 * Author: Giuseppe Callipo - IK8YFW
 * https://github.com/ik8yfw
 * 
 * 
*/
#ifndef CW1FILTER_H_
#define CW1FILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 16000 Hz

fixed point precision: 8 bits

* 0 Hz - 200 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = n/a

* 500 Hz - 1200 Hz
  gain = 4
  desired ripple = 5 dB
  actual ripple = n/a

* 1500 Hz - 8000 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = n/a

*/

#define CW1FILTER_TAP_NUM 81

typedef struct {
  int history[CW1FILTER_TAP_NUM];
  unsigned int last_index;
} CW1Filter;

void CW1Filter_init(CW1Filter* f);
void CW1Filter_put(CW1Filter* f, int input);
int CW1Filter_get(CW1Filter* f);

#endif
