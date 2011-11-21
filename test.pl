#ifdef 0

use warnings;
use strict;
use Tie::File;

my $bitmap = {};
$bitmap->{0} = "        l         b     ";
$bitmap->{1} = "   t  lll       bbb     ";
$bitmap->{2} = "  tt   ll        bb     ";
$bitmap->{3} = "  tt   ll        bbbbbb ";
$bitmap->{4} = "tttttt ll vv  vv bb   bb";
$bitmap->{5} = "  tt   ll v   vv bb   bb";
$bitmap->{6} = "  tt   ll  v vv  bb   bb";
$bitmap->{7} = "  t    lll  vv  bbbbbbb ";

my @cases;
for my $col (0..23) {
    my $data = 0;
    for my $row (0..7) {
        $data |= 2**$row if (substr($bitmap->{$row},$col,1) ne ' ');
    }
    push @cases, sprintf "case %d: display_writecol(0x%x); break;\n", $col, $data;
}

tie my @f, 'Tie::File', $0 or die $!;

my ($bl, $el) = (-1, -1);
for (0..$#f) {
    $bl = $_+1 if $f[$_] =~ m.^// BEGIN PERL_GENERATED.;
    $el = $_-1 if $f[$_] =~ m.^// END PERL_GENERATED. and $bl > 0;
    last if $el > 0;
}
print "$bl $el\n";
splice @f, $bl, $el-$bl+1, @cases;
untie @f;
=pod
#endif
// BEGIN PERL_GENERATED
// END PERL_GENERATED
#ifdef 0
=cut
#endif
