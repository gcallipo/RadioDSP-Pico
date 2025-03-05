/************************************************************* 
 * SimpleAudioFilter09.ino
 * Audio Filter DSP for Shorthwave Receiver
 * Based on Raspberry Pico Pi2 Processor module
 * 
 * Filter Functions :
 * 0. Audio passthrough 
 * 1..4 Filters ( SSB W / SSB N / CW w / CW N)
 * 
 * DNR NLMS Functions:
 * 0. Audio passthrough 
 * 1. DNR NLMS denoise Level 1 (low action)
 * 2. DNR NLMS denoise Level 2 (high action) 
 * 
 * With CW audio announce for mode and dnr change
 * 
 * Created: Start works on Feb 2023
 * Author: Giuseppe Callipo - IK8YFW
 * https://github.com/ik8yfw
 * 
 * License: Creative Common with attribution 
 * 
 * This program use filters built with the tFilter program
 * http://t-filter.engineerjs.com/
 * 
 * Last update: 19/01/2025
 * NOTE: This version does not works with Raspberry Pico Pi but
 *       need the new module Raspberry Pico Pi2 based on CortexM33 Architecture.
 */

#include "Arduino.h"
#include "audioIO.h"

#include "cwNote.h"
#include "AVGFilter.h"
#include "SSB1Filter.h"

#include "pico/multicore.h"
#include "dynamicFilters.h"

#include <I2S.h>
#include <ADCInput.h>

// GPIO I2S pin numbers
// TO connect the MAX98357A power Amplifier
#define I2S_BCLK 15
#define I2S_WS (I2S_BCLK+1)
#define I2S_DOUT 17
const int sampleRate = 8000;

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
#define OVER_RANGE 250

// define min and max gain for output amplification
#define MIN_GAIN   10 //20   // min gain sensitivity
#define MAX_GAIN   15 //35   // max gain sensitivity (default)
#define ADC_MAX    4  // Max denoise adc
#define ADC_MIN    3  // Min denoise adc

// globals
volatile int16_t     avg, sum, out_sample = 0;
AVGFilter     flt2;   // AVG  filter
SSB1Filter    flt0;   // CW/SSB filter

int           passInput = 0;
uint8_t       filterMode = 0;
uint8_t       nrMode = 0;
int16_t       outSample = 0;
int16_t       outSample_8k = 0;
int8_t        gainAudio = MIN_GAIN;
int8_t        gainFilter = 1;

uint8_t       audioPlay = 1;

// Check if need to boost the audio
// For safe reasons the value will be
// only at startup.
void initAudioGain(void) {

  // set the default audioGain for headphones
  gainAudio = MAX_GAIN;
  if (digitalRead(PIN_BUTTON_AUDIO_GAIN) == LOW) {
    // if the pin is connected to GND,
    // the audio gain will be decreased
    gainAudio = MIN_GAIN;
  }

}

//************************* NLMS FILTER * ALS & NOTCH ******************************************
int k=0;
int s=0;
#define BUFFER_SIZE 128
float       buffer[BUFFER_SIZE];
float       bufferOut[BUFFER_SIZE];
int16_t     bufIn[BUFFER_SIZE];

// Default is to use the normalized form of coefficient update
#define LMS_NORMALIZE

#define MAX_FIR  256
#define MAX_DELAY 16
#define DENOISE  1
#define NOTCH    2

    uint16_t what = DENOISE;     // DENOISE or NOTCH
    bool doLMS = true;

    float_t dataD[16];           // Can be made less than 16 by lengthDataD
    uint16_t kNextD = 0;
    uint16_t kOffsetD = 0;
    uint16_t lengthDataD = 4;      // Any value, 2 to MAX_DELAY

    float_t coeff[128];
#ifdef LMS_NORMALIZE
    float_t powerNorm[128];
    float_t pNorm = 0.0f;
#endif

    // dataF[] is arranged, by added variables kOffset and
    // lengthDataF, to be circular. A power-of-2 mask makes it circular.
    float_t dataF[128];           // Can be made less than 128 by lengthDataF
    float_t dataOutF = 0.0f;
    uint16_t kOffsetF = 0;
    uint16_t lengthDataF = 64;
    uint16_t kMask = 63;

    float_t beta = 0.03f;
    float_t decay = 0.995f;
    uint16_t numLeak = 0;

 uint16_t initializeLMS(uint16_t _what, uint16_t _lengthDataF, uint16_t _lengthDataD)
        {
        what = _what;
        if(what != DENOISE && what != NOTCH)  what = DENOISE;
        lengthDataF = powf(2.0f, log2f(_lengthDataF)+0.000001f);  //Make sure a power of 2
        lengthDataF = (lengthDataF>MAX_FIR ? MAX_FIR : lengthDataF);  // Limit length
        kMask = lengthDataF - 1;
        lengthDataD = _lengthDataD;
        lengthDataD = (lengthDataD>MAX_DELAY ? MAX_DELAY : lengthDataD);  // Limit length
#ifdef LMS_NORMALIZE
        for(int i=0; i<128; i++) powerNorm[i] = 0.01f;
        pNorm = 0.01f * 128.0f;
#endif
        return lengthDataF;
        }

 void setLMSParameters(float_t _beta, float_t _decay)
        {
        beta = _beta;
        if(beta>=1.0f) beta = 0.999999f;
        if(beta<0.000001) beta = 0.000001f;
        decay = _decay;
        if(decay>=1.0f) decay = 0.999999f;
        if(decay<0.000001) decay = 0.000001f;
        }

void ALSfilter() {

    uint16_t j, k;
    float_t blockDataIn, error, firOut;

      for(int i=0; i<128; i++)
        {
        blockDataIn = buffer[i];

        // Leakage on one coefficient
        coeff[numLeak] *= decay;         // Decay one coefficient
        if(++numLeak >= lengthDataF)     // Wrap around, if needed
            numLeak = 0;

        // Circular delay line to find correlated components
        dataD[kNextD] = blockDataIn;     // Get a new data point from block

#ifdef LMS_NORMALIZE
        powerNorm[i] = blockDataIn*blockDataIn;
        pNorm += powerNorm[i];
        if(i==127)
           pNorm -= powerNorm[0];
        else
           pNorm -= powerNorm[i+1];
#endif

        if(++kNextD >= lengthDataD)     // Next spot in delay line
           kNextD = 0;

        // Update the FIR.
        dataF[kOffsetF] = dataD[kNextD];   // Input FIR is output Delay
        firOut = 0.0f;
        for(j=0; j<lengthDataF; j++)       // Over all coefficients
            {
            k = (j + kOffsetF) & kMask;    // Data circular buffer
            firOut +=coeff[j]*dataF[k];
            }

        // Compute the error, the difference between the data point
        // just received and the FIR output.
        error = blockDataIn - firOut;

        // Update the coefficients
#ifdef LMS_NORMALIZE
        float_t kcf = error*beta/pNorm;
#else
        float_t kcf = error*beta;
#endif
        for(j=0; j<lengthDataF; j++)
            {
            k = (j + kOffsetF) & kMask;
            coeff[j] = coeff[j] + kcf*dataF[k];
            }

        // Move to next positions in circular data buffer via kOffsetF
        if(++kOffsetF >= lengthDataF)
            kOffsetF = 0;                   // Wrap the FIR circular buffer

        // fir out to output block
        if(what == DENOISE)
          bufferOut[i] = firOut;
        else
          bufferOut[i] = error;           // Auto-Notch
        }
        //transmit the block and be done
}
// End of ALS *******************************************************************************

int newSampleCheckOver =0;

void audioIO_loop(void)
{
  int16_t newSample = 0, sum =0;

  while(1){

    // Accumuate BUFFER_SIZE x MULT samples
    while (adcIn.available() >= 0 && k < BUFFER_SIZE) {
      newSample = adcIn.read()- ADC_BIAS - ADC_BIAS_SHIFT;
      newSampleCheckOver = newSample;

       // apply adc denoise.
       if (filterMode<3 && doLMS==false){
         AVGFilter_put(&flt2, newSample);
         newSample = AVGFilter_get(&flt2);
       }

       // Filter for SSB (fs=8 ksps)
       if (filterMode >0) {
          SSB1Filter_put(&flt0, newSample);
          newSample = SSB1Filter_get(&flt0);
          newSample = newSample * gainFilter;
       }

       bufIn[k] = newSample;
       k++;
    }


   // Processing thw whole accumulated samples
    if(k>= BUFFER_SIZE){
    
      if (doLMS==true){
            for(int i= 0 ; i< BUFFER_SIZE; i++){
                buffer[i] = (float)(bufIn[i]);
                buffer[i] = buffer[i]/32768; 
            }
              
            ALSfilter(); 

            for (int i = 0; i < BUFFER_SIZE; i++) {
                  bufIn[i] = (int16_t)(bufferOut[i]*32768); 
            }
      }

       // Send output to I2S the audio processed samples 
       for(int i= 0 ; i< BUFFER_SIZE; i++){
        int16_t  outSample= (int16_t)(bufIn[i]);
        if (doLMS == true || filterMode >0){
          outSample=outSample*(gainAudio);
        }else{
          outSample=outSample*(gainAudio/2);
        }
        if (audioPlay==1){
         i2s.write(outSample);
         i2s.write(outSample);
        }
       }

     // Reset the couter for the next processing stage
     k=0;
    }
  }
}

void announceCmd(int iCmd){

    setI2s(i2s);
    audioPlay = 0;
    if (iCmd==0){
      playNN();
    }
    else if (iCmd==1){
      playSW();
    }
    else if (iCmd==2){
      playSN();
    }
    else if (iCmd==3){
      playCW();
    }
    else if (iCmd==4){
      playCN();
    }
    else if (iCmd==5){
      playN1();
    }
    else if (iCmd==6){
      playN2();
    }

    audioPlay = 1;
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
          gainFilter=1;
          announceCmd(0);
        }
        else if (filterMode == 1){ // SSB W
          SSB1Filter_init(&flt0, ID_BANDPASS, W_HAMMING, 200, 3200, 8000.0);
          gainFilter=1;
          announceCmd(1);
        }
        else if (filterMode == 2){ // SSB N
          SSB1Filter_init(&flt0, ID_BANDPASS, W_HAMMING, 300, 2000, 8000.0);
          gainFilter=1;
          announceCmd(2);
        }
        else if (filterMode == 3){ // CW 600hz
          SSB1Filter_init(&flt0, ID_BANDPASS, W_BLACKMAN, 400, 1000, 8000.0);
          gainFilter=2;
          announceCmd(3);
        }
        else if (filterMode == 4){  // CW 300hz
          SSB1Filter_init(&flt0, ID_BANDPASS, W_BLACKMAN, 450, 750, 8000.0);
          gainFilter=3;
          announceCmd(4);
        }

        // Filter only for SSB filters  
        if (filterMode == 0){
                AVGFilter_init(&flt2, ADC_MAX);
        }
        else if (filterMode>0 && filterMode < 3){
                AVGFilter_init(&flt2, ADC_MIN);
        }
      
      }
    }

    // debounche
    val1 = digitalRead(PIN_BUTTON_NR);
    if (val1 != old_val1) {
      old_val1 = val1;

      if (val1 == LOW) {

        // Roll the filter selection
        if (nrMode == 2)
          nrMode = 0;
        else
          nrMode++;

        // Noise Reduction stage
      
        if (nrMode == 0) {
           doLMS = false;
          announceCmd(0);
        } else if (nrMode == 1) {
          doLMS = true;
          setLMSParameters(0.05f, 0.92f);
          //setLMSParameters(0.20, 0.89f); 
          announceCmd(5);
        } else if (nrMode == 2) {
          doLMS = true;
          //setLMSParameters(0.05f, 0.965f);
          setLMSParameters(0.05f, 0.89f); 
          announceCmd(6);
        } 
      }

    }

      /* Blink the builtin LED if the input signal go over range */
      if (newSampleCheckOver > OVER_RANGE) {
        gpio_put(LED_PIN, 1);
      } else {
        gpio_put(LED_PIN, 0);
      }

    sleep_ms(50);
  }
}

// general setup
void audioIO_setup() {
  
  SSB1Filter_init(&flt0, ID_BANDPASS, W_BLACKMAN, 200.0, 3400.0, 12000.0);
  AVGFilter_init(&flt2, ADC_MAX);

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

  adcIn.setBuffers(4, BUFFER_SIZE);

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

  // Analize LMS denoise 
  initializeLMS(DENOISE, 128, 8);
  setLMSParameters(0.05f, 0.999f); 
  doLMS=false;

  // start DSP processor
  audioIO_loop();
}