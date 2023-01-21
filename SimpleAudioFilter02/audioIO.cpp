
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

#include "Arduino.h"
#include "audioIO.h"

#include "SSB1Filter.h"
#include "SSB2Filter.h"
#include "CW1Filter.h"
#include "AVGFilter.h"

#include "LMS.h"

#include "hardware/adc.h"
#include "hardware/dma.h"
#include "pico/multicore.h"

#ifdef NO_MBED
   #include "hardware/pwm.h"
#endif

#define FSAMP 480000UL       // freq AD sample  = 480kHz
#define FSAMP_AUDIO 24000U   // audio freq sample    24 kHz //48kHz
#define ADC_CLOCK_DIV ((uint16_t)(48000000UL/FSAMP))  //48Mhz / 480Khz = 100 
#define BLOCK_NSAMP  3//  (FSAMP/FSAMP_AUDIO)            //block = 480k / 24k = 20 samples
#define NSAMP BLOCK_NSAMP

// PWN for dac on pin 15
#define OUT_DAC_PIN 15

// ADC Channel 0 is GPIO26
#define CAPTURE_CHANNEL 0

// controll button to select filter
#define PIN_BUTTON 14
#define LED_PIN 25

// define the maximum safe signal in input
#define OVER_RANGE 200 

// globals
dma_channel_config cfg;
uint               dma_chan;
volatile uint16_t  dac_audio;

void sample(uint16_t *capture_buf);
volatile uint8_t     decimator_ct=0;
volatile uint8_t     decimator_factor=1;
volatile int16_t     avg, sum, out_sample =0;

SSB1Filter    flt0;   // AM/SSB filter
SSB2Filter    flt;    // SSB filter
CW1Filter     flt1;   // CW  filter
AVGFilter     flt2;   // AVG  filter


LMSClass      fltLms(24,20); 
int           passInput=0;

uint8_t filterMode =0;     


 // continuous loop running for audio processing
void audioIO_loop(void)
{ 
    avg = 0;
    sum = 0;
    uint16_t cap_buf[NSAMP];

    // get NSAMP samples at FSAMP
    sample(cap_buf);

  // For debug only
#ifdef DEBUG_SERIAL  
  int  nn= micros();
#endif  
  

    // Check decimator for filtermode:
    // 0: as passthrough - no decimation
    // 1,2: DNR + DNF    - noise reduction AVERANGE NR + LMS
    // 3 for AM & SSB    - decimate by factor 1
    // 3 for SSB         - decimate by factor 1
    // 4: for CW         - decimate by factor 4
       
    decimator_factor = 1;
    if (filterMode==0 || filterMode==1 || filterMode==2  || filterMode==3 ) {
      decimator_factor = 1;  
    }
    else
    if (filterMode==4 ){
     decimator_factor = 2;  
    }
    if (filterMode==5){
      decimator_factor = 4;  
    } 


#ifdef DEBUG_SERIAL 
// char buffer[40];
// sprintf(buffer, "Value 0 %d ", cap_buf[0]);
// Serial.println(buffer);
#endif

    // averages input values 
    for (int i=0;i<NSAMP;i++) {
        sum+=((int16_t)cap_buf[i]);
    }
    avg = ((int16_t)(sum/NSAMP)-(ADC_BIAS-ADC_BIAS_SHIFT));

    /* Blink the builtin LED if the input signal go over range */   
    if (avg>OVER_RANGE){
       gpio_put(LED_BUILTIN, 1);
    }else{
      gpio_put(LED_BUILTIN, 0);
    }

      
    /*** AUDIO PROCESSING WITH FILTERING AND DECIMATIN BLOCK ***/
    decimator_ct ++;  
    if ( decimator_ct >= decimator_factor ){
          decimator_ct = 0;
    
         // passthrough - no decimation (fs=48 ksps)
         if (filterMode == 0 || filterMode == 1){ 
             out_sample = avg;

             AVGFilter_put(&flt2, avg);
             out_sample = AVGFilter_get(&flt2);

             //add the bias before exit
             out_sample = out_sample+(ADC_BIAS+ADC_BIAS_SHIFT);

            //scale output to 8 bit value from 12 bit
            out_sample = out_sample/16;
         }

         // no decimation, DNR + DNF 
         else if (filterMode == 2 ){ 
              AVGFilter_put(&flt2, avg);
              out_sample = AVGFilter_get(&flt2);
              
              out_sample = fltLms.computeLMS(out_sample, passInput);
              passInput = avg;

              //add the bias before exit
              out_sample = out_sample+(ADC_BIAS+ADC_BIAS_SHIFT);

              //scale output to 8 bit value from 12 bit
              out_sample = out_sample/16;

         }
         
         //AM/SSB - decimate : 1 (fs=48 ksps)
         else if (filterMode == 3){

             AVGFilter_put(&flt2, avg);
             out_sample = AVGFilter_get(&flt2);
   
             SSB2Filter_put(&flt, out_sample);
             out_sample = SSB2Filter_get(&flt);

             //add the bias before exit
             out_sample = out_sample+(ADC_BIAS+ADC_BIAS_SHIFT);

             //scale output to 8 bit value from 12 bit
             out_sample = out_sample/16;

         }
         
         //SSB - decimate : 2 (fs=12 ksps)
         else if (filterMode == 4){

             AVGFilter_put(&flt2, avg);
             out_sample = AVGFilter_get(&flt2);
             
             SSB1Filter_put(&flt0, out_sample);
             out_sample = SSB1Filter_get(&flt0);
             
             //add the bias before exit
             out_sample = out_sample+(ADC_BIAS+ADC_BIAS_SHIFT);

             //scale output to 8 bit value from 12 bit
             out_sample = out_sample/16;
         }
         
         //CW - decimate : 4 (fs=6 ksps)
         else if (filterMode == 5){
           
             CW1Filter_put(&flt1, avg);
             out_sample = CW1Filter_get(&flt1);

             //add the bias before exit
             out_sample = out_sample+(ADC_BIAS+ADC_BIAS_SHIFT);

             //scale output to 8 bit value from 12 bit
             out_sample = out_sample/16;

             #ifdef DEBUG_SERIAL 
             // char buffer[40];
             // sprintf(buffer, "Value %d ", out_sample);
             // sprintf(buffer, "Value adc  %d ", avg);
             // Serial.println(buffer);
            #endif   

         }

     }
   
#ifdef DEBUG_SERIAL 
  //char buffer[40];
  //sprintf(buffer, "Value %d ", capture_buf[0]);
  //sprintf(buffer, "Value adc  %d ", avg);
  //Serial.println(buffer);
#endif    

//    //add the bias before exit
//    out_sample = out_sample+(ADC_BIAS+ADC_BIAS_SHIFT);
//
//    //scale output to 8 bit value from 12 bit
//    out_sample = out_sample/16;

   // Clip to DAC range
   if (out_sample >= (int16_t)DAC_RANGE)            
              out_sample = DAC_RANGE-1;
   else if (out_sample<0)
              out_sample = 0;

// For debug only
#ifdef DEBUG_SERIAL 
   int lap = micros()-nn; 

   char buffer[40];
   sprintf(buffer, "Lap  %d ", lap);
   Serial.println(buffer);
#endif                  

   // Send values to PWM for output audio
   pwm_set_gpio_level(OUT_DAC_PIN, out_sample);

}

// take the samples from DMA
void sample(uint16_t *capture_buf) {
  adc_fifo_drain();
  adc_run(false);
      
  dma_channel_configure(
      dma_chan, 
      &cfg,
      capture_buf,    // dst
      &adc_hw->fifo,  // src
      NSAMP,          // transfer count
      true            // start immediately
      );

  //gpio_put(LED_BUILTIN, 1);
  adc_run(true);
  dma_channel_wait_for_finish_blocking(dma_chan);
}

uint8_t val,old_val = HIGH;
// check commands on core 1
void core1_commands_check() {
 
  // run on loop
  while(1) {

    // debounche
    val = digitalRead(PIN_BUTTON);
    if (val != old_val){
        old_val = val;
    
        if (val == LOW) {
    
          // Roll the filter selection
          if (filterMode==5)
              filterMode=0;
          else 
              filterMode++;
    
          // Active the filter stage
          if (filterMode==0)
              AVGFilter_init(&flt2, 2);     
          else if (filterMode==1){
              //fltLms.setMu(3);
              AVGFilter_init(&flt2, 24);
          }
          else if (filterMode==2){
              fltLms.setMu(4);
              AVGFilter_init(&flt2, 10);
          }
          else if (filterMode==3)
              AVGFilter_init(&flt2, 4);        
          else if (filterMode==4)
              AVGFilter_init(&flt2, 4);        
          else if (filterMode==5)
              AVGFilter_init(&flt2, 2);        
    
        }
    }

    sleep_ms(500);
  }
}


// general setup
void audioIO_setup() {

  SSB1Filter_init(&flt0);
  SSB2Filter_init(&flt);
  CW1Filter_init(&flt1);
  AVGFilter_init(&flt2, 2);

  gpio_init_mask(1<<LED_BUILTIN);  
  gpio_set_dir(LED_BUILTIN, GPIO_OUT); 
  gpio_put(LED_BUILTIN, 0);

  adc_gpio_init(26 + CAPTURE_CHANNEL);

  adc_init();
  adc_select_input(CAPTURE_CHANNEL);
  adc_fifo_setup(
     true,    // Write each completed conversion to the sample FIFO
     true,    // Enable DMA data request (DREQ)
     1,       // DREQ (and IRQ) asserted when at least 1 sample present
     false,   // We won't see the ERR bit because of 8 bit reads; disable.
     false     // Shift each sample to 8 bits when pushing to FIFO
     );

  // set sample rate
  adc_set_clkdiv(ADC_CLOCK_DIV);

  sleep_ms(1000);
  // Set up the DMA to start transferring data as soon as it appears in FIFO
  uint dma_chan = dma_claim_unused_channel(true);
  cfg = dma_channel_get_default_config(dma_chan);

  // Reading from constant address, writing to incrementing byte addresses
  channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
  channel_config_set_read_increment(&cfg, false);
  channel_config_set_write_increment(&cfg, true);

  // Pace transfers based on availability of ADC samples
  channel_config_set_dreq(&cfg, DREQ_ADC);

  gpio_set_function(OUT_DAC_PIN, GPIO_FUNC_PWM);

  int audio_pin_slice = pwm_gpio_to_slice_num(OUT_DAC_PIN);

  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, 1.f);
  pwm_config_set_wrap(&config, DAC_RANGE-1);
  pwm_init(audio_pin_slice, &config, true);

  pwm_set_gpio_level(OUT_DAC_PIN, 0);

  // start controller commands : 
  pinMode(PIN_BUTTON, INPUT_PULLUP);
    
  // pushbutton to select the filter on core 1
  multicore_launch_core1(core1_commands_check);
  sleep_ms(1400);

}
