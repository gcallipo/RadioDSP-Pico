#ifndef CW1FILTER_H_
#define CW1FILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 8000 Hz

fixed point precision: 16 bits

* 0 Hz - 300 Hz
  gain = 0
  desired attenuation = -60 dB
  actual attenuation = n/a

* 500 Hz - 1200 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = n/a

* 1400 Hz - 4000 Hz
  gain = 0
  desired attenuation = -60 dB
  actual attenuation = n/a

*/

#define CW1FILTER_TAP_NUM 69

typedef struct {
  int history[CW1FILTER_TAP_NUM];
  unsigned int last_index;
} CW1Filter;

void CW1Filter_init(CW1Filter* f);
void CW1Filter_put(CW1Filter* f, int input);
int CW1Filter_get(CW1Filter* f);

#endif
