#ifndef SSB1FILTER_H_
#define SSB1FILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 8000 Hz

fixed point precision: 16 bits

* 0 Hz - 2800 Hz
  gain = 1
  desired ripple = 2 dB
  actual ripple = n/a

* 3200 Hz - 4000 Hz
  gain = 0
  desired attenuation = -60 dB
  actual attenuation = n/a

*/

#define SSB1FILTER_TAP_NUM 32

typedef struct {
  int history[SSB1FILTER_TAP_NUM];
  unsigned int last_index;
} SSB1Filter;

void SSB1Filter_init(SSB1Filter* f, const int TYPE, const int WINDOW, int lowCut, int highCut, float sampleFrequency);
void SSB1Filter_put(SSB1Filter* f, int input);
int SSB1Filter_get(SSB1Filter* f);

#endif
