#include "CW1Filter.h"

static int filter_taps[CW1FILTER_TAP_NUM] = {
  13,
  12,
  14,
  -6,
  -39,
  -56,
  -40,
  1,
  38,
  47,
  25,
  2,
  12,
  60,
  98,
  63,
  -63,
  -216,
  -286,
  -194,
  37,
  278,
  381,
  278,
  37,
  -194,
  -286,
  -216,
  -63,
  63,
  98,
  60,
  12,
  2,
  25,
  47,
  38,
  1,
  -40,
  -56,
  -39,
  -6,
  14,
  12,
  13
};

void CW1Filter_init(CW1Filter* f) {
  int i;
  for(i = 0; i < CW1FILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void CW1Filter_put(CW1Filter* f, int input) {
  f->history[f->last_index++] = input;
  if(f->last_index == CW1FILTER_TAP_NUM)
    f->last_index = 0;
}

int CW1Filter_get(CW1Filter* f) {
  long long acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < CW1FILTER_TAP_NUM; ++i) {
    index = index != 0 ? index-1 : CW1FILTER_TAP_NUM-1;
    acc += (long long)f->history[index] * filter_taps[i];
  };
  return acc >> 12;
}
