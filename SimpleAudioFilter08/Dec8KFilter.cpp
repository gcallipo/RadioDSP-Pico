#include "Dec8KFilter.h"

static int filter_taps[DEC8KFILTER_TAP_NUM] = {
  -873,
  -2015,
  1541,
  11132,
  16872,
  11132,
  1541,
  -2015,
  -873
};

void Dec8KFilter_init(Dec8KFilter* f) {
  int i;
  for(i = 0; i < DEC8KFILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void Dec8KFilter_put(Dec8KFilter* f, int input) {
  f->history[f->last_index++] = input;
  if(f->last_index == DEC8KFILTER_TAP_NUM)
    f->last_index = 0;
}

int Dec8KFilter_get(Dec8KFilter* f) {
  long long acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < DEC8KFILTER_TAP_NUM; ++i) {
    index = index != 0 ? index-1 : DEC8KFILTER_TAP_NUM-1;
    acc += (long long)f->history[index] * filter_taps[i];
  };
  return acc >> 16;
}
