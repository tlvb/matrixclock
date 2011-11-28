#if 0

use warnings;
use strict;
use Tie::File;

my $bitmap = {};
$bitmap->{0} = " 000   1   222  333 4   455555  66 77777 888  999 ";
$bitmap->{1} = "0  00111  2   23   34   45     6       78   89   9"; 
$bitmap->{2} = "0  00  1      2    34   45    6        78   89   9";
$bitmap->{3} = "0 0 0  1     2   33 444445555 6666    7  888  9999";
$bitmap->{4} = "00  0  1    2      3    4    56   6  7  8   8    9";
$bitmap->{5} = "00  0  1   2   3   3    4    56   6  7  8   8   9 ";
$bitmap->{6} = " 000 1111122222 333     45555  666   7   888  99  ";
$bitmap->{7} = "                                                  ";

my @columns;
for my $col (0..(5*10-1)) {
    my $data = 0;
    for my $row (0..7) {
        $data |= 2**$row if (substr($bitmap->{$row},$col,1) ne ' ');
    }
    push @columns, sprintf "0x%02x", $data;
}

tie my @f, 'Tie::File', $0 or die $!;

my ($bl, $el) = (-1, -1);
for (0..$#f) {
    $bl = $_+1 if $f[$_] =~ m.^// BEGIN PERL_GENERATED.;
    $el = $_-1 if $f[$_] =~ m.^// END PERL_GENERATED. and $bl > 0;
    last if $el > 0;
}
splice @f, $bl, $el-$bl+1, ("const uint8_t font[] PROGMEM = {".join(', ',@columns)."};\n");
untie @f;
=pod
#endif
#define F_CPU 20000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
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
#define SHIFTLAG 3

void input_setup(void);
uint8_t input_read(void);
void buzzer_setup(void);
void buzzer_set(uint8_t beep);
void display_setup(void);
void display_seed(void);
void display_next(void);
void display_writecol(uint8_t c);

void timer_setup(void);

volatile uint8_t column = 0;
volatile uint16_t subsubminutes = 0;
volatile uint16_t subminutes = 0;
volatile uint8_t time[4] = {1,4,5,0};
volatile uint8_t time_p[4] = {0,0,0,0};
volatile uint8_t shift[4] = {0,0,0,0};

// BEGIN PERL_GENERATED
const uint8_t font[] PROGMEM = {0x3e, 0x71, 0x49, 0x47, 0x3e, 0x42, 0x42, 0x7f, 0x40, 0x40, 0x42, 0x61, 0x51, 0x49, 0x46, 0x22, 0x41, 0x49, 0x49, 0x36, 0x0f, 0x08, 0x08, 0x08, 0x7f, 0x4f, 0x49, 0x49, 0x49, 0x31, 0x3c, 0x4a, 0x49, 0x49, 0x30, 0x01, 0x01, 0x71, 0x09, 0x07, 0x36, 0x49, 0x49, 0x49, 0x36, 0x06, 0x49, 0x49, 0x29, 0x1e};
// END PERL_GENERATED

volatile uint8_t framebuffer[24];

int main(void) {

    display_setup();
    timer_setup();
    input_setup();
    buzzer_setup();
    sei();


    for (;;) {
        for (uint8_t i=0; i<5; ++i) {

            uint8_t byte0, byte1;

            byte0 = pgm_read_byte(&font[time[0]*5+i]);
            byte1 = pgm_read_byte(&font[time_p[0]*5+i]);
            framebuffer[i] = (byte0>>shift[0])|(byte1<<(11-shift[0]));

            byte0 = pgm_read_byte(&font[time[1]*5+i]);
            byte1 = pgm_read_byte(&font[time_p[1]*5+i]);
            framebuffer[6+i] = (byte0>>shift[1])|(byte1<<(11-shift[1]));

            byte0 = pgm_read_byte(&font[time[2]*5+i]);
            byte1 = pgm_read_byte(&font[time_p[2]*5+i]);
            framebuffer[13+i] = (byte0>>shift[2])|(byte1<<(11-shift[2]));

            byte0 = pgm_read_byte(&font[time[3]*5+i]);
            byte1 = pgm_read_byte(&font[time_p[3]*5+i]);
            framebuffer[19+i] = (byte0>>shift[3])|(byte1<<(11-shift[3]));

            switch (0xf-input_read()) {
                case 1:
                    buzzer_set(1);
                    break;
                case 2:
                    buzzer_set((subsubminutes&8)>>3);
                    break;
                case 4:
                    buzzer_set((subsubminutes&16)>>4);
                    break;
                case 8:
                        if (subminutes & 32)
                            buzzer_set(((subsubminutes&128)>>7)&((subsubminutes&32)>>5));
                        else
                            buzzer_set(0);
                    break;
                default:
                    buzzer_set(0);
                    break;
                }
        }

    }

}

void input_setup(void) {
    // io0 = PD3
    // io1 = PB7
    // io2 = PB0
    // io3 = PB1
    DDRD &=~ _BV(PD3);  // inputs
    PORTD |= _BV(PD3);  // pull-ups
    DDRB &=~ _BV(PB7) | _BV(PB0) | _BV(PB1);    // inputs
    PORTB |= _BV(PB7) | _BV(PB0) | _BV(PB1);    // pull-ups
}

uint8_t input_read(void) {
    return ((PIND>>3)&1) | ((PINB>>6)&2) | ((PINB<<2)&0x0c);
}

void buzzer_setup(void) {
    DDRB |= _BV(PB2);
}

void buzzer_set(uint8_t beep) {
    if (beep != 0)
        PORTB |= _BV(PB2);
    else
        PORTB &=~ _BV(PB2);
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

void timer_setup(void) {
    TCCR1B = _BV(WGM12) | _BV(CS10);
    TIMSK1 = _BV(OCIE1A);
    OCR1AH = 0x0f;
    OCR1AL = 0xa0;
}

ISR(TIMER1_COMPA_vect) {

    if (++subsubminutes == 250) {
        subsubminutes = 0;
        if (++subminutes == 1200) {
            subminutes = 0;
            time_p[3] = time[3];
            shift[3] = 12;
            if (++time[3] == 10)
                time[3] = 0;
        }
        if (time[3] == 0 && shift[3] == (12-SHIFTLAG)) {
            time_p[2] = time[2];
            shift[2] = 12;
            if (++time[2] == 6) 
                time[2] = 0;
        }
        if (time[2] == 0 && shift[2] == (12-SHIFTLAG)) {
            time_p[1] = time[1];
            shift[1] = 12;
            ++time[1];
            if (time[1] == 10 || (time[0] == 2 && time[1] == 4))
                time[1] = 0;
        }
        if (time[1] == 0 && shift[1] == (12-SHIFTLAG)) {
            time_p[0] = time[0];
            shift[0] = 12;
            if (++time[0] == 3)
                time[0] = 0;
        }


        for (uint8_t i=0; i<4; ++i)
            if (shift[i] > 0)
                --shift[i];
    }

    if (++column == 24) {
        column = 0;
        display_seed();
    }
    else {
        display_next();
    }
    display_writecol(framebuffer[column]);
}
#if 0
=cut
#endif
