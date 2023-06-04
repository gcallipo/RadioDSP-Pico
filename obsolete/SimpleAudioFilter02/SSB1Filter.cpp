#include "SSB1Filter.h"

static int filter_taps[SSB1FILTER_TAP_NUM] = {
  -62,
  -182,
  -86,
  140,
  19,
  -254,
  173,
  1028,
  1028,
  173,
  -254,
  19,
  140,
  -86,
  -182,
  -62
};

void SSB1Filter_init(SSB1Filter* f) {
  int i;
  for(i = 0; i < SSB1FILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void SSB1Filter_put(SSB1Filter* f, int input) {
  f->history[(f->last_index++) & 15] = input;
}

int SSB1Filter_get(SSB1Filter* f) {
  long long acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < SSB1FILTER_TAP_NUM; ++i) {
    acc += (long long)f->history[(index--) & 15] * filter_taps[i];
  };
  return acc >> 12;
}
