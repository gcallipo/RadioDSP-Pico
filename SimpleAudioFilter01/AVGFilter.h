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
#ifndef AVGFILTER_H_
#define AVGFILTER_H_

/* AVERAGE FILTER for SIMPLE DNR
*/

#define AVGFILTER_TAP_NUM 24

typedef struct {
  int history[AVGFILTER_TAP_NUM];
  unsigned int last_index;
  unsigned int FilterDepth;
} AVGFilter;

void AVGFilter_init(AVGFilter* f, int depth);
void AVGFilter_put(AVGFilter* f, int input);
int  AVGFilter_get(AVGFilter* f);

#endif
