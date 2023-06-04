#ifndef CW1FILTER_H_
#define CW1FILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 12000 Hz

fixed point precision: 8 bits

* 0 Hz - 400 Hz
  gain = 0.8
  desired ripple = 5 dB
  actual ripple = n/a

* 600 Hz - 1600 Hz
  gain = 2
  desired ripple = 5 dB
  actual ripple = n/a

* 2200 Hz - 6000 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = n/a

*/

#define CW1FILTER_TAP_NUM 33

typedef struct {
  int history[CW1FILTER_TAP_NUM];
  unsigned int last_index;
} CW1Filter;

void CW1Filter_init(CW1Filter* f);
void CW1Filter_put(CW1Filter* f, int input);
int CW1Filter_get(CW1Filter* f);

#endif
