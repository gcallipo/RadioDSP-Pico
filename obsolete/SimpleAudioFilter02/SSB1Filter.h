#ifndef SSB1FILTER_H_
#define SSB1FILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 12000 Hz

fixed point precision: 12 bits

* 0 Hz - 3200 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = n/a

* 4000 Hz - 6000 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = n/a

*/

#define SSB1FILTER_TAP_NUM 16

typedef struct {
  int history[SSB1FILTER_TAP_NUM];
  unsigned int last_index;
} SSB1Filter;

void SSB1Filter_init(SSB1Filter* f);
void SSB1Filter_put(SSB1Filter* f, int input);
int SSB1Filter_get(SSB1Filter* f);

#endif
