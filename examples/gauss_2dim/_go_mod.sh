#!/bin/sh


binpath=../../bin/$HOSTTYPE
exe_extension=$FZY_EXE_EXTENSION

#echo binpath: ${binpath}  
#echo exe_extension: ${exe_extension}  

rm -f gauss.dat gauss.nor train.*
perl createdat_gauss.pl > gauss.dat

rm -f gauss.nor dat_notmalize.log
${binpath}/fzynorml${exe_extension} -f1 gauss.dat -f2 gauss.nor

rm -f train.* dat_create.log
${binpath}/fzymkdat${exe_extension} -f1 gauss.nor -f2 gauss.dat -f3 train -Np 400 -d 0


######################################################################


rm -f mod_* tracefile.log

### modelization, cdim = 1
${binpath}/fzymodel${exe_extension} -f1 train.nrm -f2 train.nrm -c 1 -R 7 -L 2 -v 1000 -t 1000

### modelization, cdim = 3
${binpath}/fzymodel${exe_extension} -f1 train.nrm -f2 train.nrm -c 3 -R 7 -L 2 -v 1000 -t 1000

