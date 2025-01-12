/**
  ******************************************************************************
  * @file    dynamicFilters.c
  * @author  Giuseppe Callipo - IK8YFW - ik8yfw@libero.it
  * @version V1.0.0
  * @date    29-07-2020
  * @brief   Dynamic Calculation of FIR Filters -
  *          based on Bob Mailing routines
  *
  *
  * NOTE: This file is part of RadioDSP MINI project.
  *       See main.c file for additional project informations.
  * Platform: CortexM4
  ******************************************************************************/


#include "dynamicFilters.h"

double  fir_tmp[NUM_COEFFICIENTS];   
                       

// Temp array used for coefficient calculations which are performed in 64bit


//---------------------------------------------------------------
// Generate lowpass filter
//
// This is done by generating a sinc function and then windowing it
void wsfirLP(double h[],    // h[] will be written with the filter coefficients
             const int N,    // size of the filter (number of taps)
             const int WINDOW, // window function (W_BLACKMAN, W_HANNING, etc.)
             double fc)  // cutoff frequency
{
  int i;
  double w[N];    // window function
  double sinc[N]; // sinc function

  // 1. Generate Sinc function
  genSinc(sinc, N, fc);

  // 2. Generate Window function
    if (WINDOW == W_BLACKMAN)  // W_BLACKMAN
        wBlackman(w, N);
    else if (WINDOW == W_HANNING)
        wHanning(w, N);
    else if (WINDOW == W_HAMMING)
        wHamming(w, N);

  // 3. Make lowpass filter
  for (i = 0; i < N; i++) {
    h[i] = sinc[i] * w[i];
  }

  // Delete dynamic storage
  //delete []w;
  //delete []sinc;

  return;
}


//---------------------------------------------------------------
// Generate highpass filter
//
// This is done by generating a lowpass filter and then spectrally inverting it
void wsfirHP(double h[],    // h[] will be written with the filter coefficients
             const int N,    // size of the filter
             const int WINDOW, // window function (W_BLACKMAN, W_HANNING, etc.)
             double fc)  // cutoff frequency
{
  int i;

  // 1. Generate lowpass filter
  wsfirLP(h, N, WINDOW, fc);

  // 2. Spectrally invert (negate all samples and add 1 to center sample) lowpass filter
  // = delta[n-((N-1)/2)] - h[n], in the time domain
  for (i = 0; i < N; i++) {
    h[i] *= -1.0; // = 0 - h[i]
  }
  h[(N - 1) / 2] += 1.0; // = 1 - h[(N-1)/2]

  return;
}

//---------------------------------------------------------------
// Generate bandstop filter
//
// This is done by generating a lowpass and highpass filter and adding them
void wsfirBS(double h[],    // h[] will be written with the filter taps
             const int N,    // size of the filter
             const int WINDOW, // window function (W_BLACKMAN, W_HANNING, etc.)
             double fc1, // low cutoff frequency
             double fc2) // high cutoff frequency
{
  int i;
  double h1 [N];
  double h2 [N];

  // 1. Generate lowpass filter at first (low) cutoff frequency
  wsfirLP(h1, N, WINDOW, fc1);

  // 2. Generate highpass filter at second (high) cutoff frequency
  wsfirHP(h2, N, WINDOW, fc2);

  // 3. Add the 2 filters together
  for (i = 0; i < N; i++) {
    h[i] = h1[i] + h2[i];
  }

  // Delete dynamic memory
 // delete []h1;
//delete []h2;

  return;
}

//---------------------------------------------------------------
// Generate bandpass filter
//
// This is done by generating a bandstop filter and spectrally inverting it
void wsfirBP(double h[],        // h[] will be written with the filter taps
             const int N,            // size of the filter
             const int WINDOW,       // window function (W_BLACKMAN, W_HANNING, etc.)
             double fc1,       // low cutoff frequency
             double fc2)       // high cutoff frequency
{
  int i;

  // 1. Generate bandstop filter
  wsfirBS(h, N, WINDOW, fc1, fc2);

  // 2. Spectrally invert bandstop (negate all, and add 1 to center sample)
  for (i = 0; i < N; i++) {
    h[i] *= -1.0; // = 0 - h[i]
  }
  h[(N - 1) / 2] += 1.0; // = 1 - h[(N-1)/2]

  return;
}

//---------------------------------------------------------------
// Generate sinc function - used for making lowpass filter
void genSinc(double sinc[],   // sinc[] will be written with the sinc function
             const int N,          // size (number of taps)
             double fc)      // cutoff frequency
{
  int i;
  const double M = N - 1;
  double n;

  // Generate sinc delayed by (N-1)/2
  for (i = 0; i < N; i++) {
    if (i == M / 2.0) {
      sinc[i] = 2.0 * fc;
    }
    else {
      n = (double)i - M / 2.0;
      sinc[i] = sin(2.0 * M_PI * fc * n) / (M_PI * n);
    }
  }

  return;
}

//---------------------------------------------------------------
// Generate window function (Blackman)
void wBlackman(double w[],    // w[] will be written with the Blackman window
               const int N)        // size of the window
{
  int i;
  const double M = N - 1;

  for (i = 0; i < N; i++) {
    w[i] = 0.42 - (0.5 * cos(2.0 * M_PI * (double)i / M)) + (0.08 * cos(4.0 * M_PI * (double)i / M));
  }

  return;
}


//---------------------------------------------------------------
// Generate window function (Hanning)
void wHanning(double w[],   // w[] will be written with the Hanning window
              const int N)       // size of the window
{
  int i;
  const double M = N - 1;

  for (i = 0; i < N; i++) {
    w[i] = 0.5 * (1.0 - cos(2.0 * M_PI * (double)i / M));
  }

  return;
}


//---------------------------------------------------------------
// Generate window function (Hamming)
void wHamming(double w[],   // w[] will be written with the Hamming window
              const int N)       // size of the window
{
  int i;
  const double M = N - 1;

  for (i = 0; i < N; i++) {
    w[i] = 0.54 - (0.46 * cos(2.0 * M_PI * (double)i / M));
  }

  return;
}

//---------------------------------------------------------------
void coeffConvert(double in[], double out[], const int N) {

  for (int j = 0; j < N; j++) {
    out[j] = in[j];
  };

}

void convertCoeffToInt16(double in[], int out[], const int N){
   int acc =0;
   for (int j = 0; j < N; j++) {
    acc = (int)(in[j] * 32768.0);
    out[j] = acc;// >> 16;

    
#ifdef DEBUG_SERIAL
  char buffer[40];
  sprintf(buffer, "Value: %d",acc );
#endif
  };
}

//---------------------------------------------------------------
void lowpass(double h[], const int N, const int WINDOW, double fc) {
        wsfirLP(fir_tmp, N, WINDOW, fc );
        coeffConvert(fir_tmp, h, N);
}

//---------------------------------------------------------------
void highpass(double h[], const int N, const int WINDOW, double fc) {
        wsfirHP(fir_tmp, N, WINDOW, fc);
        coeffConvert(fir_tmp, h, N);
}

//---------------------------------------------------------------
void bandpass(double h[], const int N, const int WINDOW, double fc1, double fc2) {
        wsfirBP(fir_tmp, N, WINDOW, fc1, fc2 );
        coeffConvert(fir_tmp, h, N);
}

//---------------------------------------------------------------
void bandstop(double h[], const int N, const int WINDOW, double fc1, double fc2) {
        wsfirBS(fir_tmp, N, WINDOW, fc1, fc2 );
        coeffConvert(fir_tmp, h, N);
}


//float fs = 48000;
//---------------------------------------------------------------
void audioFilter(double h[], const int N, const int TYPE, const int WINDOW, double fc1, double fc2, double  fs ) {
  switch (TYPE) {
      case ID_LOWPASS:

                  lowpass(h, N, WINDOW, fc1/fs);
                  break;
      case ID_HIGHPASS:

                  highpass(h, N, WINDOW, fc1/fs);
                  break;
      case ID_BANDPASS:

                  bandpass(h, N, WINDOW, fc1/fs, fc2/fs);
                  break;
      case ID_BANDSTOP:

                  bandstop(h, N, WINDOW, fc1/fs, fc2/fs);
                  break;
      default:

                  break;
  }
}
