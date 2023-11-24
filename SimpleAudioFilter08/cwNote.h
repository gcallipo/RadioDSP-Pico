
#ifndef __CWNOTE_H__
#define __CWNOTE_H__

#include <I2S.h>

#ifdef __cplusplus
extern "C" {
#endif
 
// Change the setI2s by reference
// to fix bug when compile on Arduino 2.x and plugin 3.6.0
// Thanks David G4CJF for reporting the issue and solution.
void setI2s(const I2S &i2sIn); 
//void setI2s(I2S i2sIn);
void playDith();
void playDash();
void playDelay();
void playTone(int mult);
void playTest();

 void  playNN();
 void  playSW();
 void  playSN();
 void  playCW();
 void  playCN();
 void  playN1();
 void  playN2();
 

#ifdef __cplusplus
}
#endif
#endif
