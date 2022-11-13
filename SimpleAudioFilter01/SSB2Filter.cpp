#include "SSB2Filter.h"

static int filter_taps[SSB2FILTER_TAP_NUM] = {
  -1,
  -2,
  -1,
  2,
  3,
  2,
  -1,
  -3,
  -1,
  3,
  5,
  2,
  -5,
  -9,
  -2,
  17,
  36,
  45,
  36,
  17,
  -2,
  -9,
  -5,
  2,
  5,
  3,
  -1,
  -3,
  -1,
  2,
  3,
  2,
  -1,
  -2,
  -1
};

void SSB2Filter_init(SSB2Filter* f) {
  int i;
  for(i = 0; i < SSB2FILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void SSB2Filter_put(SSB2Filter* f, int input) {
  f->history[f->last_index++] = input;
  if(f->last_index == SSB2FILTER_TAP_NUM)
    f->last_index = 0;
}

int SSB2Filter_get(SSB2Filter* f) {
  long long acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < SSB2FILTER_TAP_NUM; ++i) {
    index = index != 0 ? index-1 : SSB2FILTER_TAP_NUM-1;
    acc += (long long)f->history[index] * filter_taps[i];
  };
  return acc >> 8;
}
