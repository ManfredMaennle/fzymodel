#!/bin/sh


FZYBIN=../../bin/${HOSTTYPE}/fzy2sets${FZY_EXE_EXTENSION}
LATEX=latex
DVIPDF=dvipdfm

#$FZYMODEL -h
#exit


FZYMODEL=mod_c1_r5
$FZYBIN -f1 ${FZYMODEL}.fzy
$LATEX ${FZYMODEL}
$LATEX ${FZYMODEL}
$DVIPDF ${FZYMODEL}


FZYMODEL=mod_c3_r5
$FZYBIN -f1 ${FZYMODEL}.fzy
$LATEX ${FZYMODEL}
$LATEX ${FZYMODEL}
$DVIPDF ${FZYMODEL}


$LATEX all_fzymodels
$LATEX all_fzymodels
$DVIPDF all_fzymodels


#xdvi all_fzymodels
