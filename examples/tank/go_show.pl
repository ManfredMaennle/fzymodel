#!/home/i8fs1/maennle/local/bin/perl -w

###
### go_show.pl; show results using gnuplot
###
### Manfred Maennle, July 1999
###

use strict;


##### read variables from config file
require "go_conf.pl";

my $modfilename = "$ENV{trainfile}.ogl";

my @testfiles = split(/ /, $ENV{"testfiles"});
my @extensions = split(/ /, $ENV{"extensions"});

my @xorders = split(/ /, $ENV{"xorders"});
my $yorder = $ENV{"yorder"};

my $n = $ENV{"n"};

my @consdimensions = split(/ /, $ENV{"consdimensions"});

my $go = $ENV{"go"};



my $terminal = "x11";
if ($ENV{HOSTTYPE} =~ m/^win/ ) {
    $terminal = "windows";
}

######################################################################

my $xlabel = "time t";
my $ylabel = "q out";

my $print_eps = 1;
my $print_png = 1;


######################################################################

#my @prefixlist = ("mod", "est", "sim");
my @prefixlist = ("est", "sim");

my $prefix;
my @simextensions;
my $ext;
my $filename;
my $outfilename;
my $cdim;

my $yhatcol = $n + 1;
my $ycol = $n + 2;
my $errcol = $n + 3;

my $gplfilename = "show_all.gpl";
my $message = "Hit <Return> to continue or <Ctrl>-C to quit!"; 
my $gpl_pause = 0;
if ($ENV{GPL_PAUSE} eq 1) { 
    $gpl_pause = 1;
}

print "rm -f $gplfilename\n";
if ($go) {
    system("rm", "-f", "$gplfilename");
}

open GPLFILE, ">$gplfilename";
print GPLFILE "#!/home/i8fs1/maennle/local/bin/gnuplot\n\n";
print GPLFILE "###\n### gnuplot file, created by 'go_show.pl'\n###\n";


######################################################################

$prefix = "mod";
foreach $cdim (@consdimensions) {
    ### print error development
    $filename = "${prefix}_c${cdim}_rr.err";
    if (-f $filename) {
	print GPLFILE "\nreset\nset term ${terminal}\n";
	print GPLFILE "set size 1.0, 1.0\n";
	print GPLFILE "set xrange [1:]\n";
	print GPLFILE "set xlabel \"number of rules\"\n";
	print GPLFILE "set ylabel \"RMS\"\n";
	print GPLFILE "plot \"$filename\" title \"RMS\" w linespoints\n";
	if ($gpl_pause) {
	    print GPLFILE "pause -1 \"${message}\"\n";
	}
	$filename =~ s/\./_/;
	if ($print_eps) {
	    $outfilename = "${filename}.eps";
	    print GPLFILE "set size 1.4, 1.0\n";
	    print GPLFILE "set term postscript eps \"Times-Roman\" 24\n";
	    print GPLFILE "set output \"${outfilename}\"\nreplot\n";
	}
	if ($print_png) {
	    $outfilename = "${filename}.png";
	    print GPLFILE "set term png\n";
	    print GPLFILE "set output \"${outfilename}\"\nreplot\n";
	}
    }
    ### print R2 development
    $filename = "${prefix}_c${cdim}_rr.r2";
    if (-f $filename) {
	print GPLFILE "\nreset\nset term ${terminal}\n";
	print GPLFILE "set size 1.0, 1.0\n";
	print GPLFILE "set xrange [1:]\n";
	print GPLFILE "set xlabel \"number of rules\"\n";
	print GPLFILE "set ylabel \"R2\"\n";
	print GPLFILE "plot \"$filename\" title \"R2\" w linespoints\n";
	if ($gpl_pause) {
	    print GPLFILE "pause -1 \"${message}\"\n";
	}
	$filename =~ s/\./_/;
	if ($print_eps) {
	    $outfilename = "${filename}.eps";
	    print GPLFILE "set size 1.4, 1.0\n";
	    print GPLFILE "set term postscript eps \"Times-Roman\" 24\n";
	    print GPLFILE "set output \"${outfilename}\"\nreplot\n";
	}
	if ($print_png) {
	    $outfilename = "${filename}.png";
	    print GPLFILE "set term png\n";
	    print GPLFILE "set output \"${outfilename}\"\nreplot\n";
	}

    }
}


######################################################################

foreach $prefix (@prefixlist) {
    ###
    foreach $cdim (@consdimensions) {
	####
	if (${prefix} eq "mod") {
	    @simextensions = ("");
	} 
        else {
	    @simextensions = @extensions;
	}
        foreach $ext (@simextensions) {
	    for (my $rule = 0; $rule <= 100; ++$rule){
		my $rulename = $rule;
		if ($rule == 100) {
		    $rulename = "o";
		}
		$filename = "${prefix}_${ext}c${cdim}_r${rulename}.out";
		if (-f $filename) {
		    print GPLFILE "\nreset\nset term ${terminal}\n";
		    print GPLFILE "set size 1.0, 1.0\n";
		    print GPLFILE "set xlabel \"${xlabel}\"\n";
		    print GPLFILE "set ylabel \"${ylabel}\"\n";
#	    print GPLFILE "set parametric\n";
#	    print GPLFILE "splot \"$testfilename\" w l, \"$filename\" w l\n";
		    print GPLFILE "plot \"${filename}\" u 0:${ycol}";
		    print GPLFILE " title \"${ext}original\"";
		    print GPLFILE " w l, \"${filename}\" u 0:${yhatcol}";
#		print GPLFILE " title \"${prefix}\"";
		    print GPLFILE " title \"${prefix}, c=${cdim},";
		    print GPLFILE " r=${rulename}\" w l,";
		    print GPLFILE " \"${filename}\" u 0:${errcol}";
		    print GPLFILE " title \"diff\" w l\n";
		    if ($gpl_pause) {
			print GPLFILE "pause -1 \"${message}\"\n";
		    }
		    $filename =~ s/\./_/;
		    if ($print_eps) {
			$outfilename = "${filename}.eps";
			print GPLFILE "set size 1.4, 1.0\n";
			print GPLFILE "set term postscript eps";
#		    print GPLFILE "set term postscript eps color";
			print GPLFILE " \"Times-Roman\" 24\n";
			print GPLFILE "set output \"${outfilename}\"\nreplot\n";
		    }
		    if ($print_png) {
			$outfilename = "${filename}.png";
			print GPLFILE "set term png\n";
			print GPLFILE "set output \"${outfilename}\"\nreplot\n";
		    }
		}
	    }
	} ### end foreach $ext
    } ### end foreach $cdim (@consdimensions)
}  ### end foreach $prefix (@prefixlist)

close GPLFILE;
print "gnuplot $gplfilename\n";
if ($go) {
    system("gnuplot", "$gplfilename");
}

#system("rm", "-f", "$gplfilename");
#system("rm", "-f", "est_*.eps");
#system("rm", "-f", "sim_*.eps");
