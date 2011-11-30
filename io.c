#include "io.h"

void io_buttons_setup(void) {
    /*
     * io0 = PD3
     * io1 = PB7
     * io2 = PB0
     * io3 = PB1
     */
    DDRD &=~ _BV(PD3);  // inputs
    PORTD |= _BV(PD3);  // pull-ups on
    DDRB &=~ _BV(PB7) | _BV(PB0) | _BV(PB1);    // inputs
    PORTB |= _BV(PB7) | _BV(PB0) | _BV(PB1);    // pull-ups on
}

uint8_t io_buttons_read(void) {
    return ((PIND>>3)&1) | ((PINB>>6)&2) | ((PINB<<2)&0x0c);
}

void io_buzzer_setup(void) {
    DDRB |= _BV(PB2);
}

void io_buzzer_set(uint8_t beep) {
    if (beep != 0)
        PORTB |= _BV(PB2);
    else
        PORTB &=~ _BV(PB2);
}
