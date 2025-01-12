#include "CW1Filter.h"

static int filter_taps[CW1FILTER_TAP_NUM] = {
  -55,
  -114,
  -141,
  -69,
  117,
  343,
  462,
  361,
  60,
  -271,
  -422,
  -316,
  -100,
  -38,
  -277,
  -665,
  -817,
  -410,
  494,
  1397,
  1697,
  1164,
  186,
  -493,
  -375,
  307,
  687,
  -57,
  -1910,
  -3797,
  -4206,
  -2282,
  1386,
  4953,
  6427,
  4953,
  1386,
  -2282,
  -4206,
  -3797,
  -1910,
  -57,
  687,
  307,
  -375,
  -493,
  186,
  1164,
  1697,
  1397,
  494,
  -410,
  -817,
  -665,
  -277,
  -38,
  -100,
  -316,
  -422,
  -271,
  60,
  361,
  462,
  343,
  117,
  -69,
  -141,
  -114,
  -55
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
  return acc >> 16;
}
