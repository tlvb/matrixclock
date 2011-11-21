#if 0

use warnings;
use strict;
use Tie::File;

my $bitmap = {};
$bitmap->{0} = "        l         b     ";
$bitmap->{1} = "   t  lll       bbb     ";
$bitmap->{2} = "  tt   ll        bb     ";
$bitmap->{3} = "  tt   ll vv  vv bbbbbb ";
$bitmap->{4} = "tttttt ll v   vv bb   bb";
$bitmap->{5} = "  tt   ll v   vv bb 0 bb";
$bitmap->{6} = "  tt   ll  v vv  bb   bb";
$bitmap->{7} = "  t    lll  vv  bbbbbbb ";

my @cases;
for my $col (0..23) {
    my $data = 0;
    for my $row (0..7) {
        $data |= 2**$row if (substr($bitmap->{$row},$col,1) ne ' ');
    }
    push @cases, sprintf "case %d: displaydata = 0x%x; break;\n", $col, $data;
}

tie my @f, 'Tie::File', $0 or die $!;

my ($bl, $el) = (-1, -1);
for (0..$#f) {
    $bl = $_+1 if $f[$_] =~ m.^// BEGIN PERL_GENERATED.;
    $el = $_-1 if $f[$_] =~ m.^// END PERL_GENERATED. and $bl > 0;
    last if $el > 0;
}
splice @f, $bl, $el-$bl+1, @cases;
untie @f;
=pod
#endif
#define F_CPU 20000000

#include <avr/io.h>
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

int main(void) {
    display_setup();
    
    for (;;) {
        display_seed();
        for (uint8_t i=0; i<24; ++i) {
            uint8_t displaydata;
            switch (i) {
// BEGIN PERL_GENERATED
case 0: displaydata = 0x10; break;
case 1: displaydata = 0x10; break;
case 2: displaydata = 0xfc; break;
case 3: displaydata = 0x7e; break;
case 4: displaydata = 0x10; break;
case 5: displaydata = 0x10; break;
case 6: displaydata = 0x2; break;
case 7: displaydata = 0xfe; break;
case 8: displaydata = 0xff; break;
case 9: displaydata = 0x80; break;
case 10: displaydata = 0x38; break;
case 11: displaydata = 0x48; break;
case 12: displaydata = 0x80; break;
case 13: displaydata = 0xc0; break;
case 14: displaydata = 0x78; break;
case 15: displaydata = 0x38; break;
case 16: displaydata = 0x82; break;
case 17: displaydata = 0xfe; break;
case 18: displaydata = 0xff; break;
case 19: displaydata = 0x88; break;
case 20: displaydata = 0xa8; break;
case 21: displaydata = 0x88; break;
case 22: displaydata = 0xf8; break;
case 23: displaydata = 0x70; break;
// END PERL_GENERATED
            }
            display_writecol(displaydata);
            _delay_us(500);
            display_next();
        }
    }
}

void display_setup(void) {
    DDRC |= 0x0f | _BV(ROWCLOCK) | _BV(ROWCLEAR);
    DDRD |= 0xff | _BV(ROWSEED);
    PORTC |= _BV(ROWCLEAR); // pull rowclear high and out of reset
    _delay_ms(250);
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
#if 0
=cut
#endif
