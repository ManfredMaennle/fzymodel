
#my $path = "$ENV{HOME}/diss/fzymodel";
$path = "../../bin/$ENV{HOSTTYPE}";


$ENV{"fzynorml"} = "${path}/fzynorml$ENV{FZY_EXE_EXTENSION}";
$ENV{"fzymkdat"} = "${path}/fzymkdat$ENV{FZY_EXE_EXTENSION}";
$ENV{"fzymodel"} = "${path}/fzymodel$ENV{FZY_EXE_EXTENSION} -n";
$ENV{"fzyestim"} = "${path}/fzyestim$ENV{FZY_EXE_EXTENSION} -n";
$ENV{"fzysimul"} = "${path}/fzysimul$ENV{FZY_EXE_EXTENSION} -n";

$ENV{"allfiles"} = "tank_all.dat";
$ENV{"normfile"} = "tank_all.dat.nor";

$ENV{"trainfile"} = "tank_t";
$ENV{"validfile"} = "tank_t";
$ENV{"testfiles"} = "tank_t tank_f0 tank_f1 tank_f2";
$ENV{"extensions"} = "t_ f0_ f1_ f2_";

$ENV{"xorders"} = "1";
$ENV{"yorder"} = 1;

$ENV{"max_n_rules"} = 5;

#$ENV{"mod_options"} = " -am -ph $ENV{yorder}";
$ENV{"mod_options"} = " -am -ph $ENV{yorder} -pO 3000000";

#$ENV{"est_sim_options"} = " ";
$ENV{"est_sim_options"} = " -Dph 0.0 0.015 0.015 1.0";


### compute dim of "full" consequence
my $order;
my @xorders = split(/ /, $ENV{"xorders"});
my $n = $ENV{"yorder"};
foreach $order (@xorders) {
    $n += $order;
}
$ENV{"n"} = $n;
my $cdim = $n + 1;
#$ENV{"consdimensions"} = "1 $cdim";
$ENV{"consdimensions"} = "$cdim";
#$ENV{"consdimensions"} = "1";


#$ENV{"go"} = 0;
$ENV{"go"} = 1;
1;

