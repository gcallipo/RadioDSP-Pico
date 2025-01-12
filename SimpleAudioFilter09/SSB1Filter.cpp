#include "SSB1Filter.h"
#include "dynamicFilters.h"

int filter_taps[SSB1FILTER_TAP_NUM];

void SSB1Filter_init(SSB1Filter* f, const int TYPE, const int WINDOW, int lowCut, int highCut, float sampleFrequency) {
  int i;
  double filter_taps_dub[SSB1FILTER_TAP_NUM];
  audioFilter(filter_taps_dub, SSB1FILTER_TAP_NUM, TYPE, WINDOW, lowCut, highCut, sampleFrequency);
  convertCoeffToInt16(filter_taps_dub, filter_taps, SSB1FILTER_TAP_NUM);

  for(i = 0; i < SSB1FILTER_TAP_NUM; ++i){
    f->history[i] = 0;
  }
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
  return acc >> 16;
}
