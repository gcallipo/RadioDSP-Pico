
/************************************************************* 
 *  SimpleAudioFilter02.ino
 * Audio Filter DSP for Shorthwave Receiver
 * Functions:
 * 0. Audio passthrough 
 * 1. DNR Average denoise 
 * 2. DNR + DNF Average Denoise + LMS Filter (NB & Beat eliminator)
 * 3. AM/SSB filter 6000 kHz + DNR low level
 * 4. SSB filter    3000 kHz + DNR medium level
 * 5. CW  filter    700  Hz  + DNR low level
 * 
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
 * 
 * Last update: 02/01/2022
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
