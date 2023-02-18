#ifndef DEC8KFILTER_H_
#define DEC8KFILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 16000 Hz

fixed point precision: 16 bits

* 0 Hz - 3000 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = n/a

* 6000 Hz - 8000 Hz
  gain = 0
  desired attenuation = -60 dB
  actual attenuation = n/a

*/

#define DEC8KFILTER_TAP_NUM 9

typedef struct {
  int history[DEC8KFILTER_TAP_NUM];
  unsigned int last_index;
} Dec8KFilter;

void Dec8KFilter_init(Dec8KFilter* f);
void Dec8KFilter_put(Dec8KFilter* f, int input);
int Dec8KFilter_get(Dec8KFilter* f);

#endif
