#!/home/i8fs1/maennle/local/bin/perl -w

###
### go_est_sim.pl; do estimation and simulation
###
### Manfred Maennle, July 1999
###

use strict;

##### read variables from config file
require "go_conf.pl";

my $fzyestim = $ENV{"fzyestim"};
my $fzysimul =  $ENV{"fzysimul"};

my $simfilename = "$ENV{testfile1}.nrm";

my @xorders = split(/ /, $ENV{"xorders"});
my $yorder = $ENV{"yorder"};

my $go = $ENV{"go"};


######################################################################

my $cdim;
my $call;
my $order;

### compute dim of "full" consequence
$cdim = $yorder + 1;
foreach $order (@xorders) {
    $cdim += $order;
}
my @consdimensions = (1, $cdim);

$call = "rm -f est_* sim_*";
print "$call\n";
if ($go) {
    system("sh", "-c", "$call");
}


my $ext = "1_";

foreach $cdim (@consdimensions) {
    ### best 
    my $filename = "mod_c${cdim}_ro.fzy";
    if (-f $filename && -f $simfilename) {
	$call = "${fzyestim} -f1 ${filename} -f2 ${simfilename} -e $ext";
	print "$call\n";
	if ($go) {
	    system("sh", "-c", "$call");
	}
	$call = "${fzysimul} -f1 ${filename} -f2 ${simfilename} -e $ext";
	$call .= " -d ${yorder}";
	print "$call\n";
	if ($go) {
	    system("sh", "-c", "$call");
	}
    }
    ### all others 
    for (my $rule = 0; $rule < 100; ++$rule){
	$filename = "mod_c${cdim}_r${rule}.fzy";
	if (-f $filename && -f $simfilename) {
	    $call = "${fzyestim} -f1 ${filename} -f2 ${simfilename} -e $ext";
	    print "$call\n";
	    if ($go) {
		system("sh", "-c", "$call");
	    }
	    $call = "${fzysimul} -f1 ${filename} -f2 ${simfilename} -e $ext";
	    $call .= " -d ${yorder}";
	    print "$call\n";
	    if ($go) {
		system("sh", "-c", "$call");
	    }
	}
    }
} ### end foreach $cdim (@consdimensions)



$ext = "2_";
$simfilename = "$ENV{testfile2}.nrm";

foreach $cdim (@consdimensions) {
    ### best 
    my $filename = "mod_c${cdim}_ro.fzy";
    if (-f $filename && -f $simfilename) {
	$call = "${fzyestim} -f1 ${filename} -f2 ${simfilename} -e $ext";
	print "$call\n";
	if ($go) {
	    system("sh", "-c", "$call");
	}
	$call = "${fzysimul} -f1 ${filename} -f2 ${simfilename} -e $ext";
	$call .= " -d ${yorder}";
	print "$call\n";
	if ($go) {
	    system("sh", "-c", "$call");
	}
    }
    ### all others 
    for (my $rule = 0; $rule < 100; ++$rule){
	$filename = "mod_c${cdim}_r${rule}.fzy";
	if (-f $filename && -f $simfilename) {
	    $call = "${fzyestim} -f1 ${filename} -f2 ${simfilename} -e $ext";
	    print "$call\n";
	    if ($go) {
		system("sh", "-c", "$call");
	    }
	    $call = "${fzysimul} -f1 ${filename} -f2 ${simfilename} -e $ext";
	    $call .= " -d ${yorder}";
	    print "$call\n";
	    if ($go) {
		system("sh", "-c", "$call");
	    }
	}
    }
} ### end foreach $cdim (@consdimensions)



$ext = "3_";
$simfilename = "$ENV{testfile3}.nrm";

foreach $cdim (@consdimensions) {
    ### best 
    my $filename = "mod_c${cdim}_ro.fzy";
    if (-f $filename && -f $simfilename) {
	$call = "${fzyestim} -f1 ${filename} -f2 ${simfilename} -e $ext";
	print "$call\n";
	if ($go) {
	    system("sh", "-c", "$call");
	}
	$call = "${fzysimul} -f1 ${filename} -f2 ${simfilename} -e $ext";
	$call .= " -d ${yorder}";
	print "$call\n";
	if ($go) {
	    system("sh", "-c", "$call");
	}
    }
    ### all others 
    for (my $rule = 0; $rule < 100; ++$rule){
	$filename = "mod_c${cdim}_r${rule}.fzy";
	if (-f $filename && -f $simfilename) {
	    $call = "${fzyestim} -f1 ${filename} -f2 ${simfilename} -e $ext";
	    print "$call\n";
	    if ($go) {
		system("sh", "-c", "$call");
	    }
	    $call = "${fzysimul} -f1 ${filename} -f2 ${simfilename} -e $ext";
	    $call .= " -d ${yorder}";
	    print "$call\n";
	    if ($go) {
		system("sh", "-c", "$call");
	    }
	}
    }
} ### end foreach $cdim (@consdimensions)


