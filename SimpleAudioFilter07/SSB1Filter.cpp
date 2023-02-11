#include "SSB1Filter.h"

static int filter_taps[SSB1FILTER_TAP_NUM] = {
  260,
  -239,
  -1201,
  -215,
  313,
  -605,
  379,
  121,
  -699,
  911,
  -474,
  -516,
  1504,
  -1706,
  538,
  1985,
  -5133,
  7741,
  24016,
  7741,
  -5133,
  1985,
  538,
  -1706,
  1504,
  -516,
  -474,
  911,
  -699,
  121,
  379,
  -605,
  313,
  -215,
  -1201,
  -239,
  260
};

void SSB1Filter_init(SSB1Filter* f) {
  int i;
  for(i = 0; i < SSB1FILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void SSB1Filter_put(SSB1Filter* f, int input) {
  f->history[f->last_index++] = input;
  if(f->last_index == SSB1FILTER_TAP_NUM)
    f->last_index = 0;
}

int SSB1Filter_get(SSB1Filter* f) {
  long long acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < SSB1FILTER_TAP_NUM; ++i) {
    index = index != 0 ? index-1 : SSB1FILTER_TAP_NUM-1;
    acc += (long long)f->history[index] * filter_taps[i];
  };
  return acc >> 16;
}
