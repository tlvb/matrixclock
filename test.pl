#if 0

use warnings;
use strict;
use Tie::File;

my $bitmap = {};
$bitmap->{0} = " 000    1    222   333  4   4 55555   66  77777  888   999  ";
$bitmap->{1} = "0   0 111   2   2 3   3 4   4 5      6        7 8   8 9   9 "; 
$bitmap->{2} = "0  00   1       2     3 4   4 5     6         7 8   8 9   9 ";
$bitmap->{3} = "0 0 0   1      2   333  44444 5555  6666     7   888   9999 ";
$bitmap->{4} = "00  0   1     2       3     4     5 6   6   7   8   8     9 ";
$bitmap->{5} = "00  0   1    2    3   3     4     5 6   6   7   8   8    9  ";
$bitmap->{6} = " 000  11111 22222  333      4 5555   666    7    888   99   ";
$bitmap->{7} = "                                                            ";

my @columns;
for my $col (0..23) {
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
// BEGIN PERL_GENERATED
uint8_t font[] = {0x3e, 0x71, 0x49, 0x45, 0x3e, 0x00, 0x42, 0x42, 0x7f, 0x40, 0x40, 0x00, 0x42, 0x61, 0x51, 0x49, 0x46, 0x00, 0x22, 0x49, 0x49, 0x49, 0x36, 0x00};
// END PERL_GENERATED
#if 0
=cut
#endif
