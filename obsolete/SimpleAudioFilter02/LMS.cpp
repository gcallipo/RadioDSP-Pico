

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
#include "LMS.h"

#include "Arduino.h"

LMSClass::LMSClass(int fSize, int muValue)
{
	filterSize = fSize; //how many input samples to track
	mu = muValue;
  mu_MAX = muValue;
	input = new int[filterSize](); //array of input samples
	noise = new int[filterSize](); //noise to minimize
	coefficients = new int[filterSize](); //filter coefficients
}

LMSClass::~LMSClass()
{
	filterSize = 0; //how many input samples to track
	delete[] input;//array of input samples
	delete[] noise; //noise to minimize
	delete[] coefficients; //filter coefficients
}


void LMSClass::setMu(int newMu){

  if (newMu>0 && newMu <=mu_MAX){
    mu = newMu;
    input = new int[filterSize](); //array of input samples
    noise = new int[filterSize](); //noise to minimize
   coefficients = new int[filterSize](); //filter coefficients
    
  }
  
}


void LMSClass::pushInput(int sample)
{
	for(int i = 0; i<(filterSize-1); i++)
	{
		input[i] = input[i+1];
	}
	input[filterSize-1] = sample;
}

void LMSClass::pushNoise(int sample)
{
	for(int i = 0; i<(filterSize-1); i++)
	{
		noise[i] = noise[i+1];
	}
	noise[filterSize-1] = sample;
}

int LMSClass::pullOutput()
{ 
  int out = dotProduct(coefficients, noise, filterSize);
	//compute LMS
	int e_k = input[0] - out;
	for(int i = 0; i < filterSize; i++)
	{
    int val = ((e_k/mu)*4);
		coefficients[i] += scalarMultiply(val, noise[i]);
	}
	//return error signal

// char buffer[40];
// sprintf(buffer, "Err 0 %d ", e_k);
 //Serial.println(buffer);
	return out;
}

int LMSClass::computeLMS(int inputSamp, int noiseSamp)
{
	pushInput(inputSamp);
	pushNoise(noiseSamp);
	return pullOutput();
}

int LMSClass::dotProduct(int* vector1, int* vector2, int length) //vector multiply, scaled result.
{
	int result = 0;
	for(int i = 0; i < length; i++)
	{
		result += scalarMultiply(vector1[i], vector2[i]);
	}
	return result;
}

int LMSClass::scalarMultiply(int a, int b)
{
	 // precomputed value:
	//#define Q 15
  #define Q 11
 	#define K   (1 << (Q-1))
 
	int result;
	long temp;
	temp = (long)a * (long)b; // result type is operand's type
	// Rounding; mid values are rounded up
	temp += K;
	// Correct by dividing by base
	result = temp >> Q;
	return result;
}
