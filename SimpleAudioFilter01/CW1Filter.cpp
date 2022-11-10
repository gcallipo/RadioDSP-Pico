/*
 * SimpleAudioFilter.ino
 * 
 * Created: Nov 2022
 * Author: Giuseppe Callipo - IK8YFW
 * https://github.com/ik8yfw
 * 
 * 
*/
#include "CW1Filter.h"

static int filter_taps[CW1FILTER_TAP_NUM] = {
  -1,
  -2,
  -2,
  -3,
  -3,
  -3,
  -2,
  0,
  2,
  4,
  6,
  8,
  8,
  8,
  7,
  4,
  2,
  0,
  -2,
  -2,
  -1,
  1,
  3,
  5,
  6,
  4,
  -1,
  -8,
  -17,
  -26,
  -34,
  -38,
  -37,
  -32,
  -20,
  -5,
  13,
  30,
  45,
  55,
  59,
  55,
  45,
  30,
  13,
  -5,
  -20,
  -32,
  -37,
  -38,
  -34,
  -26,
  -17,
  -8,
  -1,
  4,
  6,
  5,
  3,
  1,
  -1,
  -2,
  -2,
  0,
  2,
  4,
  7,
  8,
  8,
  8,
  6,
  4,
  2,
  0,
  -2,
  -3,
  -3,
  -3,
  -2,
  -2,
  -1
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
  return acc >> 8;
}
