@ECHO OFF


SET HOSTTYPE=win_x86_32bit
SET FZY_EXE_EXTENSION=_sigmoid


SET IDENTIFY_FZYMODELS=1



REM ######################################################################



REM sh _go_mod.sh
perl go_mod.pl

REM PAUSE
