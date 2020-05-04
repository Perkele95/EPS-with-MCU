 /* ADC Voltmeter for EPS.
   Continuously monitors voltage and current, both intput and output. */

// ------- Preamble -------- //
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>                           /* for round() and floor() */

/* Note: REF_VCC is the internal voltage. */
#define REF_VCC 1.1
                               /* measured division by voltage divider */
#define VOLTAGE_DIV_FACTOR  6

int main(void){
  while(1){
    // do nothing
  }
return 0;
}
