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
        case 0x00: voltageIn = ADC;
            break;
        case 0x01: currVoltIn1 = ADC;
            break;
        case 0x02: currVoltIn2 = ADC;
            break;
        case 0x03: voltageOut = ADC;
            break;
        case 0x04: currVoltOut1 = ADC;
            break;
        case 0x05: currVoltOut2 = ADC;
            break;
    }
    //select next channel, loop around if channel 5
    if(currentChannel == 5){
        selectADCchannel(0x00);
    else
        selectADCchannel(currentChannel+1);
    }
    ADCSRA |= 1<<ADSC;              /*  restart conversion  */
}

void selectADCchannel(uint8_t channel){
    //0xE0 is 11100000
    //0x1F is 00011111
    ADMUX = (ADMUX & 0xE0) | (channel & 0x1F);
}

int main(void) {

  // ------ variables ------ //
  volatile float voltageIn;
  volatile float currVoltIn1;        /*  currVolt is voltage measured across Rs.  */
  volatile float currVoltIn2;
  float currentIn;
  
  volatile float voltageOut;
  volatile float currVoltOut1;
  volatile float currVoltOut2;
  float currentOut;
  
  float Rsense = 0.01; // 10 milliohm resistors

  // -------- Inits --------- //
  sei();
  initADC();
  
  // ------ Event loop ------ //
  while (1) {
    while (ADCSRA & (1 << ADSC)){           /*  Wait for conversion to finish */
    }
    currentIn = (currVoltIn1-currVoltIn2)/Rsense;
    currentOut = (currVoltOut1-currVoltOut2)/Rsense;
    _delay_ms(500);
  }
  return 0;                            /* This line is never reached */
}
