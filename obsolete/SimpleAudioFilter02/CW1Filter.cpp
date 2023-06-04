#include "CW1Filter.h"

static int filter_taps[CW1FILTER_TAP_NUM] = {
  -14,
  -7,
  -11,
  -18,
  -27,
  -26,
  -4,
  38,
  79,
  90,
  60,
  10,
  -22,
  -15,
  20,
  43,
  25,
  -21,
  -55,
  -46,
  -8,
  11,
  -23,
  -89,
  -127,
  -95,
  -24,
  2,
  -70,
  -198,
  -255,
  -132,
  158,
  463,
  593,
  463,
  158,
  -132,
  -255,
  -198,
  -70,
  2,
  -24,
  -95,
  -127,
  -89,
  -23,
  11,
  -8,
  -46,
  -55,
  -21,
  25,
  43,
  20,
  -15,
  -22,
  10,
  60,
  90,
  79,
  38,
  -4,
  -26,
  -27,
  -18,
  -11,
  -7,
  -14
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
