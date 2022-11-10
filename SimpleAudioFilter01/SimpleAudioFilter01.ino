/*
 * SimpleAudioFilter.ino
 * 
 * Created: Nov 2022
 * Author: Giuseppe Callipo - IK8YFW
 * https://github.com/ik8yfw
 * 
 * 
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
