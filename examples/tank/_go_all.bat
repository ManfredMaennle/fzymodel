@ECHO OFF


SET HOSTTYPE=win_x86_32bit
SET FZY_EXE_EXTENSION=_sigmoid


REM SET IDENTIFY_FZYMODELS=0
SET IDENTIFY_FZYMODELS=1

SET GPL_PAUSE=0
REM SET GPL_PAUSE=1


REM ######################################################################



REM sh _go_mod.sh
perl go_mod.pl

perl go_est_sim.pl

perl go_show.pl


REM PAUSE
