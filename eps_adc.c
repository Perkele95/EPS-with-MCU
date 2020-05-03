/* ADC Voltmeter for EPS.
 * Continuously monitors voltage and current, both intput and output.
 */

// ------- Preamble -------- //
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>                           /* for round() and floor() */

/* Note: REF_VCC is the internal voltage. */
#define REF_VCC 1.1
                               /* measured division by voltage divider */
#define VOLTAGE_DIV_FACTOR  3.114


// -------- Functions --------- //
void initADC(void) {
  ADMUX |= (1 << REFS1);                  /* reference voltage on internal reference */
  ADCSRA |= 1<<ADIE;                            /*  enable interrupt    */
  ADCSRA |= (1 << ADPS1) | (1 << ADPS2);    /* ADC clock prescaler /64 */
  ADCSRA |= 1<<ADEN;                              /* enable ADC */
  ADCSRA |= 1<<ADSC;                          /*  start conversion  */
}

ISR(ADC_vect){
    //get current channel
    uint8_t currentChannel = ADMUX & 0x0F;
    //set variables based on channel
    switch(currentChannel){
        case 0x00: 
            voltageIn_a = voltageIn_b;    /*  Save the old value first before overwriting _b with new value  */
            voltageIn_b = ADC*2;          /*  Multiply by two since voltage divider is set to 1/2 */
            break;
        case 0x01: currVoltIn1 = ADC;
            break;
        case 0x02:
            currVoltIn2 = ADC;
            currentIn_a = currentIn_b;
            currentIn_b = (currVoltIn1-currVoltIn2)/0.01;   /*  10 milliohm resistor  */
            break;
        case 0x03: 
            voltageOut_a = voltageOut_b;
            voltageOut_b = ADC*2;
            break;
        case 0x04: currVoltOut1 = ADC;
            break;
        case 0x05:
            currVoltOut2 = ADC;
            currentout_a = currentOut_b;
            currentOut_b = (currVoltOut1-currVoltOut2)/0.01;
            break;
    }
    //select next channel, loop around if channel 5
    if(currentChannel == 5){
        selectADCchannel(0x00);
    else
        selectADCchannel(currentChannel+1);
    }
    ADCSRA |= 1<<ADSC;                      /*  restart conversion  */
}

void selectADCchannel(uint8_t channel){
    //0xE0 is 11100000
    //0x1F is 00011111
    ADMUX = (ADMUX & 0xE0) | (channel & 0x1F);
}

int main(void) {
  
  // ------ variables ------ //
  volatile float voltageIn_a;
  volatile float voltageIn_b;
  volatile float currVoltIn1;        /*  currVolt is voltage measured across Rs.  */
  volatile float currVoltIn2;
  volatile float currentIn_a;
  volatile float currentIn_b;
  
  volatile float voltageOut_a;
  volatile float voltageOut_b;
  volatile float currVoltOut1;
  volatile float currVoltOut2;
  volatile float currentOut_a;
  volatile float currentOut_b;

  // -------- Inits --------- //
  sei();
  initADC();
  
  // ------ Event loop ------ //
  while (1) {
    for (i = 0; i < 5; i++) {             /*  For loop that cycles through 6 adcs before continuing */
        while (ADCSRA & (1 << ADSC)){ 
        }
    }
    _delay_ms(200);
  }
  return 0;                            /* This line is never reached */
}
