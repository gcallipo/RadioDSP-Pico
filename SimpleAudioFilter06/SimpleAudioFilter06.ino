#include <I2S.h>
#include <ADCInput.h>
#include "CW1Filter.h"

// Create the I2S port using a PIO state machine
I2S i2s(OUTPUT);
CW1Filter     flt1;   // CW  filter

// GPIO pin numbers
#define pBCLK 15
#define pWS (pBCLK+1)
#define pDOUT 17

const int sampleRate = 8000; // minimum for UDA1334A

int16_t sample=0;

// Create inputt on Adc 0 - GPIO26
ADCInput adcIn(26);

#define ADC_RANGE  4096u
#define ADC_BIAS  (ADC_RANGE/2u)

#define ADC_BIAS_SHIFT  20 

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 1);

  //Serial.begin(115200);
  //Serial.println("I2S simple tone");

  i2s.setBCLK(pBCLK);
  i2s.setDATA(pDOUT);
  i2s.setBitsPerSample(16);

  // start I2S at the sample rate with 16-bits per sample
  if (!i2s.begin(sampleRate)) {
    //Serial.println("Failed to initialize I2S!");
    while (1); // do nothing
  }


  adcIn.setBuffers(1,16);

  adcIn.onReceive(onReceiveAudioSamples);
  
  if (!adcIn.begin(sampleRate)) {
    //Serial.println("Failed to initialize ADCInput!");
    while (1); // do nothing
  }

   CW1Filter_init(&flt1);

}


void onReceiveAudioSamples(){

   while (adcIn.available()>0){
    
    sample = adcIn.read();

    sample = (sample - ADC_BIAS -ADC_BIAS_SHIFT)*8;

     CW1Filter_put(&flt1, sample);
     sample = CW1Filter_get(&flt1);

    // write the same sample twice, once for left and once for the right channel
    i2s.write(sample);
    i2s.write(sample);

  };
  
}


void loop() {

 
// do nothing

}
