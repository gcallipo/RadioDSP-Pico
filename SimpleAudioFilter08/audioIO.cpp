
/*************************************************************
   SimpleAudioFilter08.ino
   Audio Filter DSP for Shorthwave Receiver

   Filter Functions :
   0. Audio passthrough
   1..4 filters 

   DNR Functions:
   0. Audio passthrough
   1. DNR Average denoise Level 1
   2. DNR Average denoise Level 2
   3. DNR Average denoise Level 3


   Created: Feb 2023
   Author: Giuseppe Callipo - IK8YFW
   https://github.com/ik8yfw

   License: Creative Common with attribution

   This program use filters built with the tFilter program
   http://t-filter.engineerjs.com/

   Last update: 18/02/2023
*/

#include "Arduino.h"
#include "audioIO.h"

#include "SSB1Filter.h"
#include "AVGFilter.h"
#include "Dec8KFilter.h"

#include "pico/multicore.h"
#include "dynamicFilters.h"

#include <I2S.h>
#include <ADCInput.h>

// GPIO I2S pin numbers
// TO connect the MAX98357A power Amplifier
#define I2S_BCLK 15
#define I2S_WS (I2S_BCLK+1)
#define I2S_DOUT 17
const int sampleRate = 16000;

// Create the I2S port using a PIO state machine
I2S i2s(OUTPUT);

// Create inputt on Adc 0 - GPIO26
ADCInput adcIn(26);

// controll button to select filter
#define PIN_BUTTON_FL 14
// controll button to select nr
#define PIN_BUTTON_NR 13
// control to se the audio gain
#define PIN_BUTTON_AUDIO_GAIN 11

// Over range onboard led
// #define LED_PIN 25
// Over range external led
#define LED_PIN 12

// define the maximum safe signal in input
// the led blink at 1/10 of maximum input level
// this assures no distorsion or overload.
// the 12 bit range ADC is -2048 to +2047 (+- 1.5 V, 3V pp)
// The over range bling at about 150 mV of input peak signal
// (300 mV pp) this is a good safe guard for 
// the Pico ADC.
#define OVER_RANGE 200

// define min and max gain for output amplification
#define MIN_GAIN   2   // suitable for headphone
#define MAX_GAIN   30  // suitable for speaker

// globals
volatile uint8_t     decimator_ct = 0;
volatile uint8_t     decimator_factor = 1;
volatile int16_t     avg, sum, out_sample = 0;

SSB1Filter    flt0;   // AM/SSB filter
AVGFilter     flt2;   // AVG  filter
Dec8KFilter   fltDec1;
Dec8KFilter   fltDec2;

int           passInput = 0;
uint8_t       filterMode = 0;
uint8_t       nrMode = 0;
int16_t       outSample = 0;
int16_t       outSample_8k = 0;
int8_t        gainAudio = MIN_GAIN;
int8_t        gainFilter = 1;

// Check if need to boost the audio
// For safe reasons the value will be
// only at startup.
void initAudioGain(void) {

  // set the default audioGain for headphones
  gainAudio = MIN_GAIN;
  if (digitalRead(PIN_BUTTON_AUDIO_GAIN) == LOW) {
    // if the pin is connected to GND,
    // the audio gain will be set to 25, suitable
    // to drive loud a 4 to 8ohm 3W speaker
    // to allow this, the MAX amplifier need a Power supply
    // of 5V (1,5 A)
    gainAudio = MAX_GAIN;
  }

}

// continuous loop running for audio processing
void audioIO_loop(void)
{
  int16_t newSample = 0;

  // For debug only
#ifdef DEBUG_SERIAL
  int  nn = micros();
#endif


#ifdef DEBUG_SERIAL
  //char buffer[40];
  //sprintf(buffer, "Value 0 %d ", cap_buf[0]);
  //Serial.println(buffer);
#endif

  while (adcIn.available() > 0) {

    newSample = adcIn.read();
    newSample = newSample - ADC_BIAS - ADC_BIAS_SHIFT;

    /* Blink the builtin LED if the input signal go over range */
    if (newSample > OVER_RANGE) {
      gpio_put(LED_PIN, 1);
    } else {
      gpio_put(LED_PIN, 0);
    }
    newSample = newSample * gainAudio;

    // Pre Filter for Decimator Anti Aliasing
    // a decimator factor by 2 need a 8kHz pre filter
    if (decimator_factor == 2) {
      Dec8KFilter_put(&fltDec1, newSample);
      newSample = Dec8KFilter_get(&fltDec1);
    }

    // apply the NR.
    AVGFilter_put(&flt2, newSample);
    newSample = AVGFilter_get(&flt2);

    // Main Filter Banks
    /// AUDIO PROCESSING WITH FILTERING AND DECIMATIN BLOCK
    decimator_ct ++;
    if ( decimator_ct >= decimator_factor ) {
      decimator_ct = 0;

      // passthrough - no decimation (fs=16 ksps)
      if (filterMode == 0 && decimator_factor == 1) {
        outSample = newSample;
      }

      // Filter for SSB (fs=8 ksps)
      if (filterMode >0 && decimator_factor == 2) {

        SSB1Filter_put(&flt0, newSample);
        outSample_8k = SSB1Filter_get(&flt0);
      }

    };

    // Post Filter for Interpolation Anti Aliasing
    // a decimator factor by 2 need a 8kHz pre filter
    if (decimator_factor == 2) {
      Dec8KFilter_put(&fltDec2, outSample_8k);
      outSample = Dec8KFilter_get(&fltDec2);
    } 

    int16_t  outSample2= outSample * gainFilter;

    // write the same sample twice, once for left and once for the right channel
    i2s.write(outSample2);
    i2s.write(outSample2);

  };

}


uint8_t val, old_val = HIGH;
uint8_t val1, old_val1 = HIGH;
// check commands on core 1
void core1_commands_check() {

  // run on loop
  while (1) {

    // debounche
    val = digitalRead(PIN_BUTTON_FL);
    if (val != old_val) {
      old_val = val;

      if (val == LOW) {

        // Roll the filter selection
        if (filterMode == 4)
          filterMode = 0;
        else
          filterMode++;

        // Active the filter stage
        if (filterMode == 0){
          decimator_factor = 1;
          gainFilter=1;
        }
        else if (filterMode == 1){ // SSB W
          decimator_factor = 2;
          SSB1Filter_init(&flt0, ID_BANDPASS, W_HAMMING, 200, 3400, 8000.0);
          gainFilter=5;
        }
        else if (filterMode == 2){ // SSB N
          decimator_factor = 2;
          SSB1Filter_init(&flt0, ID_BANDPASS, W_HAMMING, 300, 1800, 8000.0);
          gainFilter=5;
        }
        else if (filterMode == 3){ // CW 600hz
          decimator_factor = 2;
          SSB1Filter_init(&flt0, ID_BANDPASS, W_BLACKMAN, 400, 1000, 8000.0);
          gainFilter=6;
        }
        else if (filterMode == 4){  // CW 300hz
          decimator_factor = 2;
          SSB1Filter_init(&flt0, ID_BANDPASS, W_BLACKMAN, 450, 750, 8000.0);
          gainFilter=8;
        }
      }
    }

    // debounche
    val1 = digitalRead(PIN_BUTTON_NR);
    if (val1 != old_val1) {
      old_val1 = val1;

      if (val1 == LOW) {

        // Roll the filter selection
        if (nrMode == 3)
          nrMode = 0;
        else
          nrMode++;

        // Noise Reduction stage
        if (nrMode == 0) {
          AVGFilter_init(&flt2, 2);
        } else if (nrMode == 1) {
          AVGFilter_init(&flt2, 5);
        } else if (nrMode == 2) {
          AVGFilter_init(&flt2, 10);
        } else if (nrMode == 3) {
          AVGFilter_init(&flt2, 15);
        }

      }
    }

    sleep_ms(500);
  }
}


// general setup
void audioIO_setup() {

  Dec8KFilter_init(&fltDec1);
  Dec8KFilter_init(&fltDec2);
  SSB1Filter_init(&flt0, ID_BANDPASS, W_BLACKMAN, 200.0, 3400.0, 8000.0);
  AVGFilter_init(&flt2, 2);

  gpio_init_mask(1 << LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  gpio_put(LED_PIN, 0);

  i2s.setBCLK(I2S_BCLK);
  i2s.setDATA(I2S_DOUT);
  i2s.setBitsPerSample(16);

  // start I2S at the sample rate with 16-bits per sample
  if (!i2s.begin(sampleRate)) {

#ifdef DEBUG_SERIAL
    Serial.println("Failed to initialize I2S!");
#endif
    while (1); // do nothing
  }

  adcIn.setBuffers(4, 16);
  adcIn.onReceive(audioIO_loop);

  if (!adcIn.begin(sampleRate)) {
    //Serial.println("Failed to initialize ADCInput!");
    while (1); // do nothing
  }

  // start controller commands :
  pinMode(PIN_BUTTON_FL, INPUT_PULLUP);
  pinMode(PIN_BUTTON_NR, INPUT_PULLUP);
  pinMode(PIN_BUTTON_AUDIO_GAIN, INPUT_PULLUP);

  // set the maximum audio gain
  initAudioGain();

  // pushbutton to select the filter on core 1
  multicore_launch_core1(core1_commands_check);
  sleep_ms(1400);

}
