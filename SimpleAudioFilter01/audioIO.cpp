/************************************************************* 
 *  SimpleAudioFilter.ino
 * Audio Filter DSP for Shorthwave Receiver
 * Functions:
 * 1. Audio passthrough 
 * 2. SSB filter 3000 kHz + DNR low level
 * 3. CW  filter 800  Hz
 * 4. DNR High level
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
 */

#include "Arduino.h"
#include "audioIO.h"

#include "SSB2Filter.h"
#include "CW1Filter.h"
#include "AVGFilter.h"

#include "hardware/adc.h"
#include "hardware/dma.h"
#include "pico/multicore.h"

#define FSAMP 480000UL       // freq AD sample  = 480kHz
#define FSAMP_AUDIO 48000U   // audio freq sample   48kHz
#define ADC_CLOCK_DIV ((uint16_t)(48000000UL/FSAMP))  //48Mhz / 480Khz = 100 
#define BLOCK_NSAMP    (FSAMP/FSAMP_AUDIO)            //block = 480k / 48k = 10 samples
#define NSAMP BLOCK_NSAMP

// PWN for dac on pin 15
#define OUT_DAC_PIN 15

// ADC Channel 0 is GPIO26
#define CAPTURE_CHANNEL 0

// controll button to select filter
#define PIN_BUTTON 14
#define LED_PIN 25

// define the maximum safe signal in input
#define OVER_RANGE 75 

// globals
dma_channel_config cfg;
uint               dma_chan;
volatile uint16_t  dac_audio;

void sample(uint8_t *capture_buf);
volatile uint8_t     decimator_ct=0;
volatile uint8_t     decimator_factor=1;
volatile int16_t     avg, sum, out_sample =0;

SSB2Filter    flt;   // SSB filter
CW1Filter     flt1;  // CW  filter
AVGFilter     flt2;  // AVG  filter

uint8_t filterMode =0;

 // continuous loop running for audio processing
void audioIO_loop(void)
{ 
    avg = 0;
    sum = 0;
    uint8_t cap_buf[NSAMP];

    // get NSAMP samples at FSAMP
    sample(cap_buf);


    // Check decimator for filtermode:
    // 0: as passthrough - no decimation
    // 1: for SSB        - decimate by factor 2
    // 2: for CW         - decimate by factor 4
    // 3: DNR            - noise reduction AVERANGE NR
    
    decimator_factor = 1;
    if (filterMode==1){
      decimator_factor = 2;  
    }else
    if (filterMode==2){
      decimator_factor = 4;  
    } 
   
    // averages input values 
    for (int i=0;i<NSAMP;i++) {
        sum+=((int16_t)cap_buf[i]-(DAC_BIAS>>AVG_BIAS_SHIFT));
    }
    avg = ((int16_t)sum/(NSAMP));

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
    
         //passthrough - no decimation (fs=48 ksps)
         if (filterMode == 0 || filterMode == 3){ 
             out_sample = avg;
         }
         
         //SSB - decimate : 2 (fs=24 ksps)
         else if (filterMode == 1){
             SSB2Filter_put(&flt, avg);
             out_sample = SSB2Filter_get(&flt)*2;
         }

         //CW - decimate : 4 (fs=12 ksps)
         else if (filterMode == 2){
             CW1Filter_put(&flt1, avg);
             out_sample = CW1Filter_get(&flt1)*2;
         }   
    
         //add the bias before exit
         out_sample = out_sample+(DAC_BIAS>>AVG_BIAS_SHIFT);
        
     }

   AVGFilter_put(&flt2, out_sample);
   out_sample = AVGFilter_get(&flt2);

   // Clip to DAC range
   if (out_sample >= (int16_t)DAC_RANGE)            
              out_sample = DAC_RANGE-1;
   else if (out_sample<0)
              out_sample = 0;

   // Send values to PWM for output audio
   pwm_set_gpio_level(OUT_DAC_PIN, out_sample);

}

// take the samples from DMA
void sample(uint8_t *capture_buf) {
  adc_fifo_drain();
  adc_run(false);
      
  dma_channel_configure(dma_chan, &cfg,
      capture_buf,    // dst
      &adc_hw->fifo,  // src
      NSAMP,          // transfer count
      true            // start immediately
      );

  //gpio_put(LED_BUILTIN, 1);
  adc_run(true);
  dma_channel_wait_for_finish_blocking(dma_chan);
  //gpio_put(LED_BUILTIN, 0);
}

// check commands on core 1
void core1_commands_check() {
 
  // run on loop
  while(1) {

    if (digitalRead(PIN_BUTTON) == LOW) {
      
      if (filterMode==3) 
          filterMode=0;
      else 
          filterMode++;

      if (filterMode==3)
          AVGFilter_init(&flt2, 24);
      if (filterMode==2)
          AVGFilter_init(&flt2, 1);
      if (filterMode==1)
          AVGFilter_init(&flt2, 4);        
      if (filterMode==0)
          AVGFilter_init(&flt2, 1);     
    }

    sleep_ms(300);
  }
}


// general setup
void audioIO_setup() {

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
     true     // Shift each sample to 8 bits when pushing to FIFO
     );

  // set sample rate
  adc_set_clkdiv(ADC_CLOCK_DIV);

  sleep_ms(1000);
  // Set up the DMA to start transferring data as soon as it appears in FIFO
  uint dma_chan = dma_claim_unused_channel(true);
  cfg = dma_channel_get_default_config(dma_chan);

  // Reading from constant address, writing to incrementing byte addresses
  channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
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
