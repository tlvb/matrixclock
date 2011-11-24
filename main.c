#define F_CPU 20000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>


/*
 * R0..3 on PC0..3
 * R4..7 on PD4..7
 *
 *
 */

#define ROWSEED PD2
#define ROWCLOCK PC5
#define ROWCLEAR PC4

void display_setup(void);
void display_seed(void);
void display_next(void);
void display_writecol(uint8_t c);

volatile uint8_t column = 0;
volatile uint16_t subseconds = 0;
volatile uint8_t seconds = 0, minutes = 0, hours = 0;
int main(void) {

    DDRB |= _BV(PB1);

    TCCR1B = _BV(WGM12) | _BV(CS10);
    TIMSK1 = _BV(OCIE1A);
    OCR1AH = 0x0f;
    OCR1AL = 0xa0;
    sei();

    display_setup();
    uint8_t displaydata = 0x0f;

    for (;;) {
        if (column > 0 && column < 7)
            displaydata = hours;
        else if (column > 8 && column < 15) 
            displaydata = minutes;
        else if (column > 16 && column < 23) 
            displaydata = seconds;
        else
            displaydata = 0;
        display_writecol(displaydata);
    }
}

void display_setup(void) {
    DDRC |= 0x0f | _BV(ROWCLOCK) | _BV(ROWCLEAR);
    DDRD |= 0xff | _BV(ROWSEED);
    PORTC |= _BV(ROWCLEAR); // pull rowclear high and out of reset
}

void display_seed(void) {
    PORTD &=~ _BV(ROWSEED); // pull rowseed low
    PORTC |= _BV(ROWCLOCK); // pull rowclock high
    PORTC &=~ _BV(ROWCLOCK); // pull rowclock low
    PORTD |= _BV(ROWSEED); // pull rowseed high
}

void display_next(void) {
    PORTC |= _BV(ROWCLOCK); // pull rowclock high
    PORTC &=~ _BV(ROWCLOCK); // pull rowclock low
}

void display_writecol(uint8_t c) {
    PORTC &= 0xf0;
    PORTC |= (c & 0x0f);
    PORTD &= 0x0f;
    PORTD |= (c & 0xf0);
}

ISR(TIMER1_COMPA_vect) {

    PINB |= _BV(PB1);
    if (++subseconds == 5000) {
        subseconds = 0;
        if (++seconds == 60) {
            seconds = 0;
            if (++minutes == 60) {
                minutes = 0;
                if (++hours == 0) {
                    hours = 0;
                }
            }
        }
    }
    if (++column == 24) {
        column = 0;
        display_seed();
    }
    else {
        display_next();
    }
}
