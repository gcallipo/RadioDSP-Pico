#include "SSB2Filter.h"

static int filter_taps[SSB2FILTER_TAP_NUM] = {
  -51,
  -126,
  -28,
  128,
  -24,
  -236,
  209,
  1000,
  1000,
  209,
  -236,
  -24,
  128,
  -28,
  -126,
  -51
};

void SSB2Filter_init(SSB2Filter* f) {
  int i;
  for(i = 0; i < SSB2FILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void SSB2Filter_put(SSB2Filter* f, int input) {
  f->history[(f->last_index++) & 15] = input;
}

int SSB2Filter_get(SSB2Filter* f) {
  long long acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < SSB2FILTER_TAP_NUM; ++i) {
    acc += (long long)f->history[(index--) & 15] * filter_taps[i];
  };
  return acc >> 12;
}
