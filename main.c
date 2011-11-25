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
splice @f, $bl, $el-$bl+1, ("uint8_t font[] = {".join(', ',@columns)."};\n");
untie @f;
=pod
#endif
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

void timer_setup(void);

volatile uint8_t column = 0;
volatile uint16_t subseconds = 0;
volatile uint8_t seconds = 0;
volatile uint8_t digits[4] = {0,0,0,0};
volatile uint8_t digits_n[4] = {1,1,1,1};
volatile uint8_t shift[4];

// BEGIN PERL_GENERATED
uint8_t font[] = {0x3e, 0x71, 0x49, 0x47, 0x3e, 0x42, 0x42, 0x7f, 0x40, 0x40, 0x42, 0x61, 0x51, 0x49, 0x46, 0x22, 0x49, 0x49, 0x49, 0x36, 0x0f, 0x08, 0x08, 0x08, 0x7f, 0x4f, 0x49, 0x49, 0x49, 0x31, 0x3c, 0x4a, 0x49, 0x49, 0x30, 0x01, 0x01, 0x71, 0x09, 0x07, 0x36, 0x49, 0x49, 0x49, 0x36, 0x06, 0x49, 0x49, 0x29, 0x1e};
// END PERL_GENERATED

volatile uint8_t framebuffer[24];

int main(void) {

    display_setup();
    timer_setup();
    sei();
    for (;;) {

        framebuffer[0] = digits[0];
        framebuffer[1] = digits_n[0];

        framebuffer[3] = digits[1];
        framebuffer[4] = digits_n[1];

        framebuffer[6] = digits[2];
        framebuffer[7] = digits_n[2];

        framebuffer[9] = digits[3];
        framebuffer[10] = digits_n[3];

        framebuffer[14] = seconds;
        framebuffer[16] = subseconds >> 8;
        framebuffer[17] = subseconds & 0xff;
/*
        for (uint8_t i=0; i<5; ++i) {
            framebuffer[i] = font[digits[0]*5+i];
            framebuffer[i+6] = font[digits[1]*5+i];
            framebuffer[i+13] = font[digits[2]*5+i];
            framebuffer[i+19] = font[digits[3]*5+i];
        }
*/
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

void timer_setup(void) {
    TCCR1B = _BV(WGM12) | _BV(CS10);
    TIMSK1 = _BV(OCIE1A);
    OCR1AH = 0x0f;
    OCR1AL = 0xa0;
}

ISR(TIMER1_COMPA_vect) {


    if (++subseconds == 5000) {
        subseconds = 0;
        if (++seconds == 60) {
            seconds = 0;
            if (++digits[3] == 10) {
                digits[3] = 0;
                if (++digits[2] == 6) {
                    digits[2] = 0;
                    ++digits[1];
                    if (digits[1] == 4) {
                        if (digits[0] == 2) {
                            digits[1] = 0;
                            digits[0] = 0;
                        }
                    }
                    else if (digits[1] == 10) {
                        ++digits[0];
                        digits[1] = 0;
                    }
                }
            }
            for (uint8_t i=0; i<4; ++i)
                digits_n[i] = digits[i]+1;
            if (digits_n[3] == 10) {
                digits_n[3] = 0;
                if (digits_n[2] == 6) {
                    digits_n[2] = 0;
                    digits_n[1];
                    if (digits_n[1] == 4) {
                        if (digits_n[0] == 2) {
                            digits_n[1] = 0;
                            digits_n[0] = 0;
                        }
                    }
                    else if (digits_n[1] == 10) {
                        digits_n[1] = 0;
                    }
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
    display_writecol(framebuffer[column]);
}
#if 0
=cut
#endif
