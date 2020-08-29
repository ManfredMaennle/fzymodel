
#my $path = "$ENV{HOME}/diss/fzymodel";
$path = "../../bin/$ENV{HOSTTYPE}";


$ENV{"fzynorml"} = "${path}/fzynorml$ENV{FZY_EXE_EXTENSION}";
$ENV{"fzymkdat"} = "${path}/fzymkdat$ENV{FZY_EXE_EXTENSION}";
$ENV{"fzymodel"} = "${path}/fzymodel$ENV{FZY_EXE_EXTENSION} -n";
$ENV{"fzyestim"} = "${path}/fzyestim$ENV{FZY_EXE_EXTENSION} -n";
$ENV{"fzysimul"} = "${path}/fzysimul$ENV{FZY_EXE_EXTENSION} -n";


$ENV{"originalfile"} = "turbo_tsl.dat";

$ENV{"trainfile"} = "turbo_t";
$ENV{"validfile"} = "turbo_t";
#$ENV{"testfile"} = "turbo_s";
$ENV{"testfile1"} = "turbo_s";
#$ENV{"testfile1"} = "turbo_l";
$ENV{"testfile2"} = "turbo_l";
#$ENV{"testfile2"} = "";
$ENV{"testfile3"} = "turbo_t";

$ENV{"xorders"} = "3 3";
$ENV{"yorder"} = 2;
# $ENV{"xorders"} = "4 4";
# $ENV{"yorder"} = 3;

$ENV{"simextensions"} = "1_ 2_ 3_";

$ENV{"max_n_rules"} = 9;

#$ENV{"mod_options"} = "-am -ph $ENV{yorder}";
$ENV{"mod_options"} = " -am -ph $ENV{yorder} -pO 3000000";

#$ENV{"go"} = 0;
$ENV{"go"} = 1;
1;

