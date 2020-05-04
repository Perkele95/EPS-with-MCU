// PWM for the eps with mcu.

#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 16000000UL

void initPWM(void){
    // PLL allows for internal 64 MHz clock for timer1
    PLLCSR = (1 << PLLE);                   /*  Enable PLL for timer1 */
    _delay_ms(0.1ms);                     /*  Wait for PPL to stabilize */
    while(!(PLLCSR & (1 << PLOCK)));            /* PLL Lock Detector  */
    PLLCSR = (1 << PCKE);                           /* PCK Enable */
    
    // PWM on OC1A, or pin 6
    TCCR1 |= (1 << PWM1A) | (1 << COM1A1);     /*  non-inverting PWM mode  */
    TCCR1 |= (1 << CS11) | (1 << CS10);     /*  The datasheet says these need to be set if 100 kHz is desired */
    OCR1C = 159;
    OCR1A = OCR1C / 2;      /*  50% DC. OC1A is set when the counter reaches OCR1A  */
}

int main(void)
{
    /* ---- Inits ----- */
    initPWM();
    
    while (1){
        // do nothing
    }
    return 0;
}
