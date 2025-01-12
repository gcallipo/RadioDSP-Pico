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
/*
  Windowed Sinc FIR Generator
  Bob Maling (BobM.DSP@gmail.com)
  Contributed to musicdsp.org Source Code Archive
  Last Updated: April 12, 2005

  http://www.musicdsp.org/showone.php?id=194

  Usage:
    Lowpass:  wsfirLP(h, N, WINDOW, fc)
    Highpass: wsfirHP(h, N, WINDOW, fc)
    Bandpass: wsfirBP(h, N, WINDOW, fc1, fc2)
    Bandstop: wsfirBS(h, N, WINDOW, fc1, fc2)

  where:
    h (double[N]):  filter coefficients will be written to this array
    N (int):    number of taps
    WINDOW (int): Window (W_BLACKMAN, W_HANNING, or W_HAMMING)
    fc (double):  cutoff (0 < fc < 0.5, fc = f/fs)
            --> for fs=48kHz and cutoff f=12kHz, fc = 12k/48k => 0.25

    fc1 (double): low cutoff (0 < fc < 0.5, fc = f/fs)
    fc2 (double): high cutoff (0 < fc < 0.5, fc = f/fs)


  Windows included here are Blackman, Hanning, and Hamming. Other windows can be
  added by doing the following:
    1. "Window type constants" section: Define a global constant for the new window.
    2. wsfirLP() function: Add the new window as a case in the switch() statement.
    3. Create the function for the window

       For windows with a design parameter, such as Kaiser, some modification
       will be needed for each function in order to pass the parameter.
*/
#ifndef WSFIR_H
#define WSFIR_H

#define _USE_MATH_DEFINES  
#include <math.h> 
#include "Arduino.h" 
 
#define DEBUG_SERIAL
#define NUM_COEFFICIENTS 64

// ID's for the different types of filters
#define ID_LOWPASS   1
#define ID_HIGHPASS  2
#define ID_BANDPASS  3
#define ID_BANDSTOP  4


// ID's for window type contstants
static const int W_BLACKMAN =  1;
static const int W_HANNING =   2;
static const int W_HAMMING =   3;


// Function prototypes
void audioFilter(double h[], const int N, const int TYPE, const int WINDOW,  double fc1,  double fc2, double fc);
void bandpass(double h[], const int N, const int WINDOW, double fc1,  double fc2);
void wsfirLP(double h[], const int N, const int WINDOW, double fc);
void wsfirHP(double h[], const int N, const int WINDOW, double fc);
void wsfirBS(double h[], const int N, const int WINDOW, double fc1,  double fc2);
void wsfirBP(double h[], const int N, const int WINDOW,  double fc1, double fc2);
void genSinc(double sinc[], const int N, double fc);
void wBlackman(double w[], const int N);
void wHanning(double w[], const int N);
void wHamming(double w[], const int N);
void convertCoeffToInt16(double in[], int out[], const int N);


#endif
