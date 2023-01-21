#ifndef CW1FILTER_H_
#define CW1FILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 6000 Hz

fixed point precision: 12 bits

* 0 Hz - 100 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = n/a

* 200 Hz - 1000 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = n/a

* 1100 Hz - 3000 Hz
  gain = 0
  desired attenuation = -40 dB
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
