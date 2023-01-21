#ifndef SSB2FILTER_H_
#define SSB2FILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 24000 Hz

fixed point precision: 12 bits

* 0 Hz - 6000 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = n/a

* 8000 Hz - 12000 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = n/a

*/

#define SSB2FILTER_TAP_NUM 16

typedef struct {
  int history[SSB2FILTER_TAP_NUM];
  unsigned int last_index;
} SSB2Filter;

void SSB2Filter_init(SSB2Filter* f);
void SSB2Filter_put(SSB2Filter* f, int input);
int SSB2Filter_get(SSB2Filter* f);

#endif
