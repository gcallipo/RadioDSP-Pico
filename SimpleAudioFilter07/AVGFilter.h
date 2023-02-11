
/************************************************************* 
 * SimpleAudioFilter07.ino
 * Audio Filter DSP for Shorthwave Receiver
 * 
 * Filter Functions :
 * 0. Audio passthrough 
 * 1. AM/SSB filter 6000 kHz 
 * 2. SSB filter    3000 kHz
 * 3. CW  filter    700  Hz
 * 
 * DNR Functions:
 * 0. Audio passthrough 
 * 1. DNR Average denoise Level 1
 * 2. DNR Average denoise Level 2
 * 3. DNR Average denoise Level 3
 * 
 * 
 * Created: Feb 2023
 * Author: Giuseppe Callipo - IK8YFW
 * https://github.com/ik8yfw
 * 
 * License: Creative Common with attribution 
 * 
 * This program use filters built with the tFilter program
 * http://t-filter.engineerjs.com/
 * 
 * Last update: 11/02/2023
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
