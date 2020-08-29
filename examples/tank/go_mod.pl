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
my $fzyestim = $ENV{"fzyestim"};
my $fzysimul = $ENV{"fzysimul"};

my $allfiles = $ENV{"allfiles"};
my $normfile = $ENV{"normfile"};

my $trainfile = $ENV{"trainfile"};
my $validfile = $ENV{"validfile"};
my @testfiles = split(/ /, $ENV{"testfiles"});
my @extensions = split(/ /, $ENV{"extensions"});

my @xorders = split(/ /, $ENV{"xorders"});
my $yorder = $ENV{"yorder"};

my @consdimensions = split(/ /, $ENV{"consdimensions"});

my $max_n_rules = $ENV{"max_n_rules"};

my $mod_options = $ENV{"mod_options"};

my $go = $ENV{"go"};


$ENV{"fault_simulation"} = 0;


######################################################################


my $call;
my $cdim;
my $filename;
my $ext;
my $k;


######################################################################

### clean up data files
$call = "rm -f *.dat *.nor *.nrm *.ogl dat_*.log";
print "$call\n";
if ($go) {
    system("sh", "-c", "$call");
}

##### make data files
$k = 0;
foreach $filename (@testfiles) {
    print "make file `${filename}.dat'\n";
    $ENV{"mkdat_outfile"} = "${filename}.dat";
    my $faultsim = $k - 1;
    if ($faultsim < 0) {
	$faultsim = 0;
    }
    $ENV{"fault_simulation"} = $faultsim;
    $call = "perl go_mkdat.pl";
    if ($go) {
	system("sh", "-c", "$call");
    }
    ++$k;
}

##### prepare data collection
$call = "cat ${trainfile}.dat > ${allfiles}";
print "$call\n";
if ($go) {
    system("sh", "-c", "$call");
}

##### compute normalization
$call = "${fzynorml} -f1 ${allfiles} -f2 ${normfile} -sd 2.5";
print "$call\n";
if ($go) {
    system("sh", "-c", "$call");
}

##### create the normalized and extended data
$k = 0;
foreach $filename (@testfiles) {
    print "make normalized data file `${filename}.nrm'\n";
    $call = "${fzymkdat} -f1 ${normfile} -f2 ${filename}.dat -f3 ${filename}";
    $call .= " -d @{xorders} ${yorder} -rt";
    print "$call\n";
    if ($go) {
	system("sh", "-c", "$call");
    }
    ++$k;
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
