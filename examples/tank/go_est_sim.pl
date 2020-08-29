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
$fzyestim .= $ENV{"est_sim_options"};
$fzysimul .= $ENV{"est_sim_options"};

my @testfiles = split(/ /, $ENV{"testfiles"});
my @extensions = split(/ /, $ENV{"extensions"});

my @xorders = split(/ /, $ENV{"xorders"});
my $yorder = $ENV{"yorder"};

my @consdimensions = split(/ /, $ENV{"consdimensions"});

my $go = $ENV{"go"};


######################################################################

my $cdim;
my $call;
my $filename;
my $k;

######################################################################

$call = "rm -f est_* sim_*";
print "$call\n";
if ($go) {
    system("sh", "-c", "$call");
}


$k = 0;
foreach $filename (@testfiles) {
    my $ext = "-e $extensions[$k]";
    print "********** estimation and simulation of file `$filename'\n";
    foreach $cdim (@consdimensions) {
	for (my $rule = 0; $rule <= 100; ++$rule){
	    my $model = "mod_c${cdim}_r${rule}.fzy";
	    if ($rule == 100) {
		$model = "mod_c${cdim}_ro.fzy";
	    }
	    if (-f $model && -f "${filename}.nrm") {
		$call = "${fzyestim} -f1 ${model} -f2 ${filename}.nrm $ext";
		print "$call\n";
		if ($go) {
		    system("sh", "-c", "$call");
		}
		$call = "${fzysimul} -f1 ${model} -f2 ${filename}.nrm $ext";
		$call .= " -d ${yorder}";
		print "$call\n";
		if ($go) {
		    system("sh", "-c", "$call");
		}
	    }
	}
    } ### end foreach $cdim (@consdimensions)
    ++$k;
} ### end $filename (@testfiles)

