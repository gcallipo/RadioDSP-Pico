/*
 * SimpleAudioFilter.ino
 * 
 * Created: Nov 2022
 * Author: Giuseppe Callipo - IK8YFW
 * https://github.com/ik8yfw
 * 
 * 
*/

#include "Arduino.h"
#include "audioIO.h"

#include "SSB2Filter.h"
#include "CW1Filter.h"

#include "hardware/adc.h"
#include "hardware/dma.h"
#include "pico/multicore.h"


#define FSAMP 480000UL       // freq AD sample  = 480kHz
#define FSAMP_AUDIO 32000U   // audio freq sample   32kHz
#define ADC_CLOCK_DIV ((uint16_t)(48000000UL/FSAMP))  //48Mhz / 480Khz = 100 
#define BLOCK_NSAMP    (FSAMP/FSAMP_AUDIO)            //block = 480k / 32k = 15 samples
#define NSAMP BLOCK_NSAMP

// PWN for dac on pin 15
#define OUT_DAC_PIN 15

// ADC Channel 0 is GPIO26
#define CAPTURE_CHANNEL 0

// controll button to select filter
#define PIN_BUTTON 14
#define LED_PIN 25

// globals
dma_channel_config cfg;
uint               dma_chan;
volatile uint16_t  dac_audio;

void sample(uint8_t *capture_buf);
volatile uint8_t     decimator_ct=0;
volatile uint8_t     decimator_factor=1;

SSB2Filter    flt;
CW1Filter     flt1;
uint8_t filterMode =1;

 // continuous loop running for audio processing
void audioIO_loop(void)
{ 
    int16_t out_sample =0;
    uint8_t cap_buf[NSAMP];

    // get NSAMP samples at FSAMP
    sample(cap_buf);

    // Check decimator filter:
    // 0: as passthrough
    // 1: for SSB
    // 2: for CW
    decimator_factor=(filterMode==0)?1:2;

/*** AUDIO PROCESSING WITH FILTERING AND DECIMATIN BLOCK ***/

    decimator_ct ++;
    if ( decimator_ct >= decimator_factor ){
          decimator_ct = 0;

     
         int16_t sum=0;
         for (int i=0;i<NSAMP;i++) {
              sum+=((int16_t)cap_buf[i]-(DAC_BIAS>>AVG_BIAS_SHIFT));
         }
         int16_t avg = ((int16_t)sum/(NSAMP));

         //passthrough - no decimation
         if (filterMode == 0){ 
             out_sample = (avg+(DAC_BIAS>>AVG_BIAS_SHIFT));
         }
         //SSB - decimate : 2
         else if (filterMode == 1){
             SSB2Filter_put(&flt, (avg+(DAC_BIAS>>AVG_BIAS_SHIFT))*2);
             out_sample = SSB2Filter_get(&flt); 
         }
         //SSB - decimate : 2
         else if (filterMode == 2){
             CW1Filter_put(&flt1, (avg+(DAC_BIAS>>AVG_BIAS_SHIFT))*3);
             out_sample = CW1Filter_get(&flt1);
         }
         
         // Clip to DAC range
         if (out_sample > (int16_t)DAC_RANGE)            
              out_sample = DAC_RANGE;
         else if (out_sample<0)
              out_sample = 0;

         // Send values to PWM for output audio
         pwm_set_gpio_level(OUT_DAC_PIN, out_sample);
  }

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

  gpio_put(LED_BUILTIN, 1);
  adc_run(true);
  dma_channel_wait_for_finish_blocking(dma_chan);
  gpio_put(LED_BUILTIN, 0);
}

// check commands on core 1
void core1_commands_check() {
 
  // run on loop
  while(1) {

    if (digitalRead(PIN_BUTTON) == LOW) {
      if (filterMode==2) filterMode=0;
      else filterMode++;
    }

    sleep_ms(500);
  }
}


// general setup
void audioIO_setup() {

  SSB2Filter_init(&flt);
  CW1Filter_init(&flt1);

  gpio_init_mask(1<<LED_BUILTIN);  
  gpio_set_dir(LED_BUILTIN, GPIO_OUT); 

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
  //adc_set_clkdiv(0);
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


 
