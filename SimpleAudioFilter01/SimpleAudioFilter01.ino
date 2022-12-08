/************************************************************* 
 *  SimpleAudioFilter.ino
 * Audio Filter DSP for Shorthwave Receiver
 * Functions:
 * 0. Audio passthrough 
 * 1. DNR High level
 * 2. AM/SSB filter 6000 kHz + DNR low level
 * 3. SSB filter    3000 kHz + DNR medium level
 * 4. CW  filter    700  Hz  + DNR low level
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
 *  Last update: 08/12/2022
 */

#include "audioIO.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  gpio_init_mask(1<<LED_BUILTIN);  
  gpio_set_dir(LED_BUILTIN, GPIO_OUT); 

  audioIO_setup();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void loop(void)
{
  audioIO_loop();
}
