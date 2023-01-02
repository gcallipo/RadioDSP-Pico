

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
/* Arduino LMS Algorithm */
/*
This library will implement a simple LMS Noise cancelling algorithm

W_k+1 = W_k + 2 * mu * e_k * X_k
e_k = d_k - W_k^T * X_k

*/


#ifndef _LMS_H_INCLUDED	//prevent DSP Shield library from being invoked twice and breaking the namespace
#define _LMS_H_INCLUDED

#define defualtFilterSize 56
#define defaultMu 4
class LMSClass {
	public:
		int 	filterSize; //how many input samples to track
		int*	input; //array of input samples
		int*	noise; //noise to minimize
		int*	coefficients; //filter coefficients
		int 	mu; //weighting factor

    int   mu_MAX;
    

		LMSClass(int fSize = defualtFilterSize, int muValue=defaultMu); //sets filter size, clears buffers
		~LMSClass(); //clears and deallocates buffers
    void setMu(int newMu);
		void pushInput(int sample); //add new input sample
		void pushNoise(int sample); //add new noise sample
		int pullOutput(); //returns e_k, the most recent sample output, updates weights
		int computeLMS(int inputSamp, int noiseSamp);
	//private:
		int dotProduct(int* vector1, int* vector2, int length); //vector multiply, scaled result.
		int scalarMultiply(int a, int b); //scalar multiply, scaled result
	};
#endif
