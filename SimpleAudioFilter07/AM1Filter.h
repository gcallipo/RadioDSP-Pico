#ifndef AM1FILTER_H_
#define AM1FILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 16000 Hz

fixed point precision: 16 bits

* 0 Hz - 5500 Hz
  gain = 1
  desired ripple = 2 dB
  actual ripple = n/a

* 6500 Hz - 8000 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = n/a

*/

#define AM1FILTER_TAP_NUM 23

typedef struct {
  int history[AM1FILTER_TAP_NUM];
  unsigned int last_index;
} AM1Filter;

void AM1Filter_init(AM1Filter* f);
void AM1Filter_put(AM1Filter* f, int input);
int AM1Filter_get(AM1Filter* f);

#endif
