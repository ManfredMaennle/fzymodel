#!/home/i8fs1/maennle/local/bin/perl -w

###
###
### Manfred Maennle, March 2000
###

use strict;


##### read variables from config file
require "go_conf.pl";
my $outfile = $ENV{"mkdat_outfile"};
my $faultsimulation = $ENV{"fault_simulation"};
print "********** fault simulation = $faultsimulation!\n";

#$outfile = "ttt";
my $n_patterns = 1000;
my $first_pattern = 0;
my $mean_length = 10;


my $a_out_0 = 0.01;
my $a_out = $a_out_0;
my $cq_in = 0.12;
my $A = 1;
my $h_0 = 2.0;


if ($outfile =~ /_f/) {
    $n_patterns = 1000;
    $first_pattern = 500;
    srand(4);
}
# elsif ($faultsimulation == 2) {
#     $n_patterns = 1000;
#     $first_pattern = 500;
#     srand(3);
# }
else {
    srand(1);
}

######################################################################
### constants and functions

my $pi = 4 * atan2(1, 1);
my $g = 9.81;

### output file
my $out = \*STDOUT;
if (defined($outfile)) {
    open ($out, "> ${outfile}") 
	or die "Cannot open output file `${outfile}'!\n";
}

### signum
sub sgn { 
    if ($_[0] < 0) {
	return -1;
    }
    if ($_[0] > 0) {
	return 1;
    }
    return 0;
}

### tangens
sub tan {
    return sin($_[0]) / cos($_[0]);
}

###
sub expdev {
    my $equaldev = rand();
    while ($equaldev == 0.0) {
	$equaldev = rand();
    }
    return -log($equaldev);
}

### q_out(h)
sub q_out {
    my $h = $_[0];
    if ($h <= 0) {
	return 0;
    }
    return $a_out * sqrt(2*$g*$h);
}

### q_in(phi)
sub q_in {
    my $phi = $_[0];
    return $cq_in * sin($phi);

}

### create_phi(k): valve angel
my $next_new_phi = 0;
my $new_phi = 0;
sub create_phi {
    my $k = $_[0];
    if ($k < 50) {
	return 0.0;
    }
    if ($k < 100) {
	return $pi/2;
    }
    if ($k < 200) {
	return 0.0;
    }
    if ($k < 300) {
	return $pi/6;
    }
    if ($k < 320) {
	return 0.0;
    }
    if ($k < 340) {
	return $pi/2;
    }
    if ($k < 360) {
	return 0.0;
    }
    if ($k < 380) {
	return $pi/2;
    }
    if ($k < 400) {
	return 0.0;
    }
    if ($k < 500) {
	return $pi/3;
    }    
    if ($k > $next_new_phi) {
	$next_new_phi = $k + 1 + int($mean_length * expdev());
	#$new_phi = $pi * 0.5 * rand();
	$new_phi = $pi * 0.9 * rand();
	if ($new_phi < 0) {
	    $new_phi = 0;
	}
	if ($new_phi > $pi/2) {
	    $new_phi = $pi/2;
	}
    }
    return $new_phi;
}

######################################################################

my $dt = 1.0;

my $h = $h_0;
my $t = 0;

my $drift = 0;
### development of h, etc., under a certain excitation phi

for (my $k = $first_pattern; $k < $n_patterns; ++$k) {
    $t += $dt;
    my $phi = create_phi($k);
    my $qin = q_in($phi);
    if($faultsimulation == 1 and $k >= 750) {
	### sudden 0.3 % valve leakage
	$qin += 0.003 * $cq_in;
    }
    my $qout = q_out($h);
    $h += ($qin - $qout) / $A;
    my $measured_h = $h;
    if($faultsimulation == 2 and $k >= 750 and $k < 850) {
	### drift fault of 0.75 % sensor error
	$drift += 0.0075 * 0.01;
    }
#    print $out "$qin $h $qout\n";
#    print $out "$phi $qout $h\n";
    $measured_h = (1 - $drift) * $h;
    print $out "$phi $measured_h\n";
}

1;
