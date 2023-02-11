#include "AM1Filter.h"

static int filter_taps[AM1FILTER_TAP_NUM] = {
  -830,
  -1042,
  818,
  -325,
  -543,
  1397,
  -1543,
  410,
  2027,
  -5091,
  7646,
  24133,
  7646,
  -5091,
  2027,
  410,
  -1543,
  1397,
  -543,
  -325,
  818,
  -1042,
  -830
};

void AM1Filter_init(AM1Filter* f) {
  int i;
  for(i = 0; i < AM1FILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void AM1Filter_put(AM1Filter* f, int input) {
  f->history[f->last_index++] = input;
  if(f->last_index == AM1FILTER_TAP_NUM)
    f->last_index = 0;
}

int AM1Filter_get(AM1Filter* f) {
  long long acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < AM1FILTER_TAP_NUM; ++i) {
    index = index != 0 ? index-1 : AM1FILTER_TAP_NUM-1;
    acc += (long long)f->history[index] * filter_taps[i];
  };
  return acc >> 16;
}
