
#include "cwNote.h"
#include <I2S.h>

const int frequency = 1000; // frequency of square wave in Hz
const int amplitude = 2000; // amplitude of square wave

const int sampleRate = 16000; 
const int halfWavelength = (sampleRate / frequency); // half wavelength of square wave

I2S i2s_;

void setI2s(const I2S &i2sIn){
  i2s_ = i2sIn;
}

void playDith(){
  playTone(1);
}

void playDash(){
  playTone(4);
}

void playDelay(){
  delay(100);
}


void playTone(int mult){

  int16_t sample = amplitude; // current sample value
  int count = 0;

  for (int i = 1; i<= (360*mult); i++){

      if (count % halfWavelength == 0) {
        // invert the sample every half wavelength count multiple to generate square wave
        sample = -1 * sample;
      }
    
      // write the same sample twice, once for left and once for the right channel
      i2s_.write(sample);
      i2s_.write(sample);
    
      // increment the counter for the next sample
      count++;

  }

  delay(85);
  
}

void playTest(){
}

void playCW(){
// C
  playDash();
  playDith();
  playDash();
  playDith();

  playDelay();

// W
  playDith();
  playDash();
  playDash();
  playDelay();
  
}

void playCN(){
// C
  playDash();
  playDith();
  playDash();
  playDith();

  playDelay();

// N

  playDash();
  playDith();
  playDelay();
  
}

void playSW(){

// S
  playDith();
  playDith();
  playDith();

  playDelay();

// S
  playDith();
  playDith();
  playDith();

  playDelay();
  

// W
  playDith();
  playDash();
  playDash();
  playDelay();
  
}

void playSN(){

// S
  playDith();
  playDith();
  playDith();

  playDelay();

// S
  playDith();
  playDith();
  playDith();

  playDelay();

// N

  playDash();
  playDith();
  playDelay();
  
}

void playNN(){

// N

  playDash();
  playDith();
  playDelay();

  playDelay();

// N

  playDash();
  playDith();
  playDelay();
  
}


void playN1(){

// N

  playDash();
  playDith();
  
  playDelay();

// 1
  playDith();
  playDash();
  playDash();
  playDash();
  playDash();

  playDelay();
  
}


void playN2(){

// N

  playDash();
  playDith();
  playDelay();

// 2
  playDith();
  playDith();
  playDash();
  playDash();
  playDash();

  playDelay();
  
}
