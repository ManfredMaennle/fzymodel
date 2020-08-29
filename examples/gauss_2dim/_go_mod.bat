@ECHO OFF


SET HOSTTYPE=win_x86_32bit
SET FZY_EXE_EXTENSION=_sigmoid


REM rm -f gauss.dat
REM perl createdat_gauss.pl > gauss.dat
REM %binpath%\fzymkdat_sigmoid.exe -f1 gauss.dat -f2 train -Np 400 -d 0


REM ######################################################################



sh _go_mod.sh

REM PAUSE
