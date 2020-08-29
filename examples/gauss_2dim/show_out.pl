#!/home/i8fs1/maennle/local/bin/perl

###
### show_mod.pl; show modelization results using gnuplot
###
### Manfred Maennle, May 1999, March 2018
###

$datfilename = "train.nrm";
@consdimensions = (1, 3);

$print_terminal = 0;   ### confirm each gnuplot result in terminal; set 1 == yes, 0 == batch mode
$print_eps = 1;        ### print gnuplot result in eps file; set 1 == yes, 0 == no
$print_png = 1;        ### print gnuplot result in png file; set 1 == yes, 0 == no


######################################################################

### determine operating system for gnuplot terminal setting; default is Unix/X11
$operating_system = $^O;
# print STDERR "$0: operating system: $operating_system\n"; exit 0;

$terminal = "x11";
if (($operating_system  eq "MSWin32") or ($operating_system  eq "msys")) {
    $terminal = "windows";
}


$gplfilename = "show_out.gpl";
$message = "Hit <Return> to continue or <Ctrl>-C to quit!"; 

system("rm", "-f", "$gplfilename");
open GPLFILE, ">$gplfilename";
print GPLFILE "#!/home/i8fs1/maennle/local/bin/gnuplot\n\n";
print GPLFILE "###\n### gnuplot file, created by 'show_out.pl'\n###\n";


foreach $cdim (@consdimensions) {
    ### print error development
    $filename = "mod_c${cdim}_ra.err";
    if (-f $filename) {
	print GPLFILE "\nreset\nset term ${terminal}\n";
	print GPLFILE "plot \"$filename\" w linespoints\n";
	if ($print_terminal) {
	    print GPLFILE "pause -1 \"${message}\"\n";
	}
	$filename =~ s/\./_/;
	if ($print_eps) {
	    $outfilename = "${filename}.eps";
	    print GPLFILE "set term postscript eps \"Times-Roman\" 24\n";
	    print GPLFILE "set output \"${outfilename}\"\nreplot\n";
	}
	if ($print_png) {
	    $outfilename = "${filename}.png";
	    print GPLFILE "set term png\n";
	    print GPLFILE "set output \"${outfilename}\"\nreplot\n";
	}
    }
    ### print error development
    $filename = "mod_c${cdim}_ra.r2";
    if (-f $filename) {
	print GPLFILE "\nreset\nset term ${TERMINAL}\n";
	print GPLFILE "plot \"$filename\" w linespoints\n";
	if ($print_terminal) {
	    print GPLFILE "pause -1 \"${message}\"\n";
	}
	$filename =~ s/\./_/;
	if ($print_eps) {
	    $outfilename = "${filename}.eps";
	    print GPLFILE "set term postscript eps \"Times-Roman\" 24\n";
	    print GPLFILE "set output \"${outfilename}\"\nreplot\n";
	}
	if ($print_png) {
	    $outfilename = "${filename}.png";
	    print GPLFILE "set term png";
	    print GPLFILE "set output \"${outfilename}\"\nreplot\n";
	}
    }
    ### print all model outputs
    for ($rule = 0; $rule < 100; ++$rule){
	$filename = "mod_c${cdim}_r${rule}.out";
	if (-f $filename && -f $datfilename) {
	    print GPLFILE "\nreset\nset term ${terminal}\n";
	    print GPLFILE "set parametric\n";
	    # print GPLFILE "splot \"$datfilename\" w l, \"$filename\" w l\n";
	    print GPLFILE "splot \"$filename\" w l\n";
	    if ($print_terminal) {
		print GPLFILE "pause -1 \"${message}\"\n";
	    }
	    $filename =~ s/\./_/;
	    if ($print_eps) {
		$outfilename = "${filename}.eps";
		print GPLFILE "set term postscript eps \"Times-Roman\" 24\n";
		print GPLFILE "set output \"${outfilename}\"\nreplot\n";
	    }
	    if ($print_png) {
		$outfilename = "${filename}.png";
		print GPLFILE "set term png\n";
		print GPLFILE "set output \"${outfilename}\"\nreplot\n";
	    }
	}
    } ### end print all model outputs
} ### end foreach $cdim (@consdimensions)

close GPLFILE;
system("gnuplot", "$gplfilename");

#system("rm", "-f", "$gplfilename");
#system("rm", "-f", "mod_*.eps");
