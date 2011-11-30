#include "display.h"

void display_setup(void) {
    DDRC |= 0x0f | _BV(ROWCLOCK) | _BV(ROWCLEAR);
    DDRD |= 0xff | _BV(ROWSEED);
    PORTC |= _BV(ROWCLEAR);     // pull rowclear high and out of reset
}

void display_seedcol(void) {
    PORTD &=~ _BV(ROWSEED);     // pull rowseed low
    PORTC |= _BV(ROWCLOCK);     // pull rowclock high
    PORTC &=~ _BV(ROWCLOCK);    // pull rowclock low
    PORTD |= _BV(ROWSEED);      // pull rowseed high
}

void display_nextcol(void) {
    PORTC |= _BV(ROWCLOCK);     // pull rowclock high
    PORTC &=~ _BV(ROWCLOCK);    // pull rowclock low
}

void display_write(uint8_t c) {
    PORTC &= 0xf0;
    PORTC |= (c & 0x0f);
    PORTD &= 0x0f;
    PORTD |= (c & 0xf0);
}
