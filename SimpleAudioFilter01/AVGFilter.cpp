/************************************************************* 
 *  SimpleAudioFilter.ino
 * Audio Filter DSP for Shorthwave Receiver
 * Functions:
 * 1. Audio passthrough 
 * 2. SSB filter 3000 kHz + DNR low level
 * 3. CW  filter 800  Hz
 * 4. DNR High level
 * 
 * 
 * Created: Nov 2022
 * Author: Giuseppe Callipo - IK8YFW
 * https://github.com/ik8yfw
 * 
 * License: Creative Common with attribution 
 * 
 * This program use filters built with the tFilter program
 * http://t-filter.engineerjs.com/
 */

#include "AVGFilter.h"

void AVGFilter_init(AVGFilter* f, int filterDepth) {
  int i;
  for(i = 0; i < f->FilterDepth; ++i)
    f->history[i] = 50;
  f->last_index = 0;
  f->FilterDepth = filterDepth;
}

void AVGFilter_put(AVGFilter* f, int input) {
  f->history[f->last_index++] = input;
  if(f->last_index == f->FilterDepth)
    f->last_index = 0;
}

int AVGFilter_get(AVGFilter* f) {
  long long acc = 0;
  for(int i = 0; i < f->FilterDepth; ++i) {
    acc += (long long)f->history[i];
  };
  acc = (long long)(acc/f->FilterDepth);
  
  return (int)acc;
}
