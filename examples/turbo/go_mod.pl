#!/home/i8fs1/maennle/local/bin/perl -w

###
### go_mod.pl; do modeling
###
### Manfred Maennle, August 1999
###

use strict;


##### read variables from config file
require "go_conf.pl";

my $fzynorml = $ENV{"fzynorml"};
my $fzymkdat = $ENV{"fzymkdat"};
my $fzymodel = $ENV{"fzymodel"};

my $originalfile = $ENV{"originalfile"};
my $trainfile = $ENV{"trainfile"};
my $validfile = $ENV{"validfile"};
my $testfile1 = $ENV{"testfile1"};
my $testfile2 = $ENV{"testfile2"};

my @xorders = split(/ /, $ENV{"xorders"});
my $yorder = $ENV{"yorder"};

my $max_n_rules = $ENV{"max_n_rules"};

my $mod_options = $ENV{"mod_options"};

my $go = $ENV{"go"};


######################################################################

##### local conf

my $ofile_t = "turbo_t.dat";
my $ofile_s = "turbo_s.dat";
my $ofile_l = "turbo_l.dat";


######################################################################


my $call;
my $order;

### compute dim of "full" consequence
my $cdim = $yorder + 1;
foreach $order (@xorders) {
    $cdim += $order;
}

#my @consdimensions = (1, $cdim);
my @consdimensions = ($cdim);


my $normparams = "turbo_tsl.nor";

### clean up
#system("rm", "-f", "*.nrm");
$call = "rm -f *.nrm *.ogl *.log mod_* est_* sim_*";
$call .= "  ${originalfile} ${normparams}";
print "$call\n";
if ($go) {
    system("sh", "-c", "$call");
}


######################################################################

### prepare original data
$call = "cat ${ofile_t} ${ofile_l} ${ofile_s} > ${originalfile}";
print "$call\n";
if ($go) {
    system("sh", "-c", "$call");
}

##### compute normalization
$call = "${fzynorml} -f1 ${originalfile} -f2 ${normparams} -sd 2.5";
print "$call\n";
if ($go) {
    system("sh", "-c", "$call");
}

##### create the training data
$call = "${fzymkdat} -f1 ${normparams} -f2 ${trainfile}.dat -f3 ${trainfile}";
$call .= " -d @{xorders} ${yorder} -rt";
print "$call\n";
if ($go) {
    system("sh", "-c", "$call");
}

##### create the test data
$call = "${fzymkdat} -f1 ${normparams} -f2 ${testfile1}.dat -f3 ${testfile1}";
$call .= " -d @{xorders} ${yorder} -rt";
print "$call\n";
if ($go) {
    system("sh", "-c", "$call");
}
$call = "${fzymkdat} -f1 ${normparams} -f2 ${testfile2}.dat -f3 ${testfile2}";
$call .= " -d @{xorders} ${yorder} -rt";
print "$call\n";
if ($go) {
    system("sh", "-c", "$call");
}


######################################################################


### exit if no need to reidentify the models
if ($ENV{IDENTIFY_FZYMODELS} ne 1) {
    print "no identification of fuzzy models... exiting.\n";
    exit 0;
}

### clean up models
$call = "rm -f mod_* est_* sim_*";
print "$call\n";
if ($go) {
    system("sh", "-c", "$call");
}


##### do the modeling
foreach $cdim (@consdimensions) {
    $call = "${fzymodel} -f1 ${trainfile}.nrm -f2 ${validfile}.nrm -c ${cdim}";
    $call .= " -R ${max_n_rules} ${mod_options}";
    print "$call\n";
    if ($go) {
	system("sh", "-c", "$call");
    }
} ### end foreach $cdim (@consdimensions)


