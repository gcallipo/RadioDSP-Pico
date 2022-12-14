#include "SSB2Filter.h"

static int filter_taps[SSB2FILTER_TAP_NUM] = {
  -7,
  -37,
  -53,
  -69,
  -66,
  -43,
  -6,
  28,
  43,
  30,
  -3,
  -37,
  -49,
  -28,
  16,
  57,
  66,
  29,
  -41,
  -103,
  -110,
  -29,
  131,
  324,
  481,
  541,
  481,
  324,
  131,
  -29,
  -110,
  -103,
  -41,
  29,
  66,
  57,
  16,
  -28,
  -49,
  -37,
  -3,
  30,
  43,
  28,
  -6,
  -43,
  -66,
  -69,
  -53,
  -37,
  -7
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
  return acc >> 12;
}
