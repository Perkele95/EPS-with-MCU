 /* EPS main made for ATtiny45.
   Continuously monitors voltage and current, both intput and output.
   This build has been compiled with atmel studio 7.0. */

// ------- Preamble -------- //
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* Note: REF_VCC is the internal voltage. */
#define REF_VCC 1.1
#define VOLTAGE_DIV_FACTOR  9      /* measured division by voltage divider */
#define RSENSE 0.01
#define VBAT_LOW 5.6
#define VCHARGE 7.2
#define IBAT_THRESHOLD 0.4375  /* 5% of the discharge rate of 4 batteries, or (35/20)/4 */ 

// ------ variables ------ //
volatile float voltageIn_a;
volatile float voltageIn_b;
volatile float currVoltIn;        /*  currVolt is voltage measured after Rs  */
volatile float currentIn_a;
volatile float currentIn_b;

volatile float voltageOut;
volatile float currVoltOut;
volatile float currentOut;

float d_u;
float d_i;
uint8_t chargeFlag;

// -------- Functions --------- //
void initPWM(void){
    // PLL allows for internal 64 MHz clock for timer1
    PLLCSR = (1 << PLLE);                   /*  Enable PLL for timer1 */
    _delay_ms(0.1);                     /*  Wait for PPL to stabilize */
    while(!(PLLCSR & (1 << PLOCK)));            /* PLL Lock Detector  */
    PLLCSR = (1 << PCKE);                           /* PCK Enable */
    
    // PWM on OC1A, or pin 6
    TCCR1 |= (1 << PWM1A) | (1 << COM1A1);     /*  non-inverting PWM mode  */
    TCCR1 |= (1 << CS11) | (1 << CS10);     /*  The datasheet says these need to be set if 100 kHz is desired */
    OCR1C = 159;                          /* Count TOP value */
    OCR1A = OCR1C / 10;                 /*  10% D by default  */
}

void initADC(void) {
  ADMUX |= (1 << REFS1);                  /* reference voltage on internal reference */
  ADCSRA |= ( 1 << ADIE);                            /*  enable interrupt    */
  ADCSRA |= (1 << ADPS1) | (1 << ADPS2);    /* ADC clock prescaler /64 */
  ADCSRA |= (1 << ADEN);                              /* enable ADC */
}

// This selects the next output pin based on currentChannel
void selectADCchannel(uint8_t channel){
	//0xE0 is 11100000
	//0x1F is 00011111
	ADMUX = (ADMUX & 0xE0) | (channel & 0x1F);
}

ISR(ADC_vect){
    //get current channel
    uint8_t currentChannel = ADMUX & 0x0F;
    //set variables based on channel
    switch(currentChannel){
        case 0x00: 
            voltageIn_a = voltageIn_b;    /*  Save the old value first before overwriting _b with new value  */
            voltageIn_b = ADC * REF_VCC * VOLTAGE_DIV_FACTOR / 1023;
            break;
        case 0x01:
            currVoltIn = ADC * REF_VCC * VOLTAGE_DIV_FACTOR / 1023;
            currentIn_a = currentIn_b;
            currentIn_b = (voltageIn_b - currVoltIn) / RSENSE;   /*  10 milliohm resistor  */
            break;
        case 0x02:
            voltageOut = ADC * REF_VCC * VOLTAGE_DIV_FACTOR / 1023;
            break;
        case 0x03:
            currVoltOut = ADC * REF_VCC * VOLTAGE_DIV_FACTOR / 1023;
            currentOut = (currVoltOut - voltageOut) / RSENSE;
            break;
    }
    //select next channel, loop around if channel 3
    if(currentChannel == 3){
        selectADCchannel(0x00);
    } else {
        selectADCchannel(currentChannel+1);
    }
}

// MPPT function sets the duty cycle
void MPPT(void){
  d_u = voltageIn_b - voltageIn_a;
  d_i = currentIn_b - currentIn_a;
  if(d_u == 0){
    if(d_i > 0){
        OCR1A -= 1;
      } else if(d_i < 0){
        OCR1A += 1;
      }
  } else {
      if((d_i/d_u) > (-(currentIn_b/voltageIn_b))){
        OCR1A -= 1;
      } else if((d_i/d_u) < (-(currentIn_b/voltageIn_b))){
        OCR1A += 1;
      }
  }
}

// CV mode loop is exited when battery current drops below threshold
void cvMode(void){
  while(currentOut > IBAT_THRESHOLD){
    if(voltageOut < VCHARGE){
      OCR1A += 1;
    } else if(voltageOut > VCHARGE) {
      OCR1A -= 1;
    }
    // Sample adc
    for (uint8_t k = 0; k < 3; k++) {
      ADCSRA |= (1 << ADSC);
      while (ADCSRA & (1 << ADSC)){}
    }
  }
  chargeFlag = 0;
}

int main(void) {
  // -------- Inits --------- //
  initPWM(); 
  sei();
  initADC();
  _delay_ms(100);    /* Wait for converter to stabilise */
  
  for (uint8_t i = 0; i < 3; i++) {             /*  sample adc once to set "a" values to a non-zero value */
      ADCSRA |= (1 << ADSC);                  /*  start conversion  */
      while (ADCSRA & (1 << ADSC)){}
  }
  
  // ------ Event loop ------ //
  while (1) {
    for (uint8_t j = 0; j < 3; j++) {             /*  adc sample */
        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC)){}
    }
    // State monitoring.
    // First check is battery is being charged or not.
    // If so, check the current state, whether CC, CV or charge done.
    if(chargeFlag){
        if(voltageOut >= VCHARGE){
          cvMode(); // This is CV mode
        } else {
          MPPT();   // This is CC mode
        }
    }
    // If not charging, check if charging needs to start because of low voltage.
    // Start MPPT regardless
    else{
        if(voltageOut < VBAT_LOW){
          chargeFlag = 1;
          MPPT();
        } else {
          MPPT();
        }
    }
    _delay_ms(200);
  }
return 0;
}
