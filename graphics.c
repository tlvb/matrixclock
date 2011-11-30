#if 0

use warnings;
use strict;
use Tie::File;

my $font = {};
my $symbols = {};
$font->{0} = " 000   1   222  333 4   455555  66 77777 888  999 ";
$font->{1} = "0  00111  2   23   34   45     6       78   89   9"; 
$font->{2} = "0  00  1      2    34   45    6        78   89   9";
$font->{3} = "0 0 0  1     2   33 444445555 6666    7  888  9999";
$font->{4} = "00  0  1    2      3    4    56   6  7  8   8    9";
$font->{5} = "00  0  1   2   3   3    4    56   6  7  8   8   9 ";
$font->{6} = " 000 1111122222 333     45555  666   7   888  99  ";
$font->{7} = "                                                  ";
$symbols->{0} = "    cc    ooo    nnn         ";
$symbols->{1} = "   cc    o o o  n   n x     x";
$symbols->{2} = "   cc  co  o  o n   n  x   x ";
$symbols->{3} = "   ccccco  oo o n   n   x x  ";
$symbols->{4} = "  ccccc o     o n   n    x   ";
$symbols->{5} = " ccc     o   o n     n  x x  ";
$symbols->{6} = "ccc       ooo  nnnnnnn x   x ";
$symbols->{7} = " c                n   x     x";

my @fontcolumns;
my @symbolscolumns;
for my $col (0..length($font->{0})-1) {
    my $data = 0;
    for my $row (0..7) {
        $data |= 2**$row if (substr($font->{$row},$col,1) ne ' ');
    }
    push @fontcolumns, sprintf "0x%02x", $data;
}
for my $col (0..length($symbols->{0})-1) {
    my $data = 0;
    for my $row (0..7) {
        $data |= 2**$row if (substr($symbols->{$row},$col,1) ne ' ');
    }
    push @symbolscolumns, sprintf "0x%02x", $data;
}

tie my @f, 'Tie::File', $0 or die $!;

my ($bl, $el) = (-1, -1);
for (0..$#f) {
    $bl = $_+1 if $f[$_] =~ m.^// BEGIN PERL_GENERATED.;
    $el = $_-1 if $f[$_] =~ m.^// END PERL_GENERATED. and $bl > 0;
    last if $el > 0;
}

my $fontstr = "const uint8_t font[] PROGMEM = {".join(', ',@fontcolumns)."};\n\n";
my $symbolsstr = "const uint8_t symbols[] PROGMEM = {".join(', ',@symbolscolumns)."};\n\n";
$fontstr =~ s/(.{1,70})\s/$1\n/g;
$symbolsstr =~ s/(.{1,70})\s/$1\n/g;
splice @f, $bl, $el-$bl+1, ("\n".$fontstr.$symbolsstr);
untie @f;
=pod
#endif

#include "graphics.h"

// BEGIN PERL_GENERATED

const uint8_t font[] PROGMEM = {0x3e, 0x71, 0x49, 0x47, 0x3e, 0x42,
0x42, 0x7f, 0x40, 0x40, 0x42, 0x61, 0x51, 0x49, 0x46, 0x22, 0x41,
0x49, 0x49, 0x36, 0x0f, 0x08, 0x08, 0x08, 0x7f, 0x4f, 0x49, 0x49,
0x49, 0x31, 0x3c, 0x4a, 0x49, 0x49, 0x30, 0x01, 0x01, 0x71, 0x09,
0x07, 0x36, 0x49, 0x49, 0x49, 0x36, 0x06, 0x49, 0x49, 0x29, 0x1e};

const uint8_t symbols[] PROGMEM = {0x40, 0xe0, 0x70, 0x3e, 0x1f, 0x19,
0x18, 0x0c, 0x1c, 0x22, 0x41, 0x4f, 0x49, 0x22, 0x1c, 0x60, 0x5e,
0x41, 0xc1, 0x41, 0x5e, 0x60, 0x82, 0x44, 0x28, 0x10, 0x28, 0x44,
0x82};

// END PERL_GENERATED

#if 0
=cut
#endif
