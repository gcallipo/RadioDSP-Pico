
#ifndef __CWNOTE_H__
#define __CWNOTE_H__

#include <I2S.h>

#ifdef __cplusplus
extern "C" {
#endif
 
void setI2s(I2S i2sIn);
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
