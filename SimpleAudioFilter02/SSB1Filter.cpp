#include "SSB1Filter.h"

static int filter_taps[SSB1FILTER_TAP_NUM] = {
  17,
  19,
  24,
  25,
  20,
  7,
  -13,
  -38,
  -63,
  -84,
  -92,
  -84,
  -55,
  -7,
  57,
  131,
  203,
  265,
  306,
  320,
  306,
  265,
  203,
  131,
  57,
  -7,
  -55,
  -84,
  -92,
  -84,
  -63,
  -38,
  -13,
  7,
  20,
  25,
  24,
  19,
  17
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
  return acc >> 12;
}
