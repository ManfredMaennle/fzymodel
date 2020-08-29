@ECHO OFF
rem version 01, 2011-09-13

SET CURRENT_GO_BAT=

:SETENVIRONMENT
SET CURRENT_DIR=%CD%
CD \programs-usb
START /B /WAIT set_path_add_programs-usb.bat

rem PAUSE
EXIT



