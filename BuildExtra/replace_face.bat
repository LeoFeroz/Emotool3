@echo off

:: This function is to replace a face (including edge) with another face in the same .emo
:: Parameters REPLACE_SRC and REPLACE_DST are the index of the groups. They match the number in exported .objs folder

:: Example, replace face 0, with face 2
set REPLACE_SRC=0x2
set REPLACE_DST=0x0

emotool3 ReplaceFace %1 %REPLACE_SRC% %REPLACE_DST%
set /p=Press ENTER to close this window.
