#!/home/i8fs1/maennle/local/bin/perl

# Copyright (c) 1999 Manfred Maennle.
#
# Permission to use, copy, modify, and distribute this software and
# its documentation for educational and scientific purpose is hereby
# granted without fee, provided that the above copyright notice
# appear in all copies and that both that copyright notice and this
# permission notice appear in supporting documentation.  
# Manfred Maennle makes no representations about the suitability of 
# this software for any purpose. It is provided "as is" without express 
# or implied warranty.  
#
# $Id:$
#




use strict;


my $h = 2.0;
my $i;
my $j;


for($i = 0; $i < 20; ++$i) { 
    for($j = 0; $j < 20; ++$j) { 
	my $x = $j / 5.0;
	my $y = $i / 5.0;
	my $z = exp( $h * (-($x-2)*($x-2)-($y-1)*($y-1)) )
	    + exp( $h * (-($x-2)*($x-2)-($y-3)*($y-3)) );
	print "$x $y $z\n";
    }
    print "\n";
}

