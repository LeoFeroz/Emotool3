@echo off

:: This function performs a LOGICAL comparison of two files
:: If files are considered equal by the program, it can be considered that the game will read them exactly in the same way,
:: even if the binary files are ordered in a different way (eg. non binary identical, but equivalent)
::
:: This can also be used to compare binary and xml files, since emotool3 treats them in the same way
::
:: Formats supported: emo, emb, emm and 2ry.
::

set file1=HLD_00.2ry
set file2=HLD_00.ps3.2ry

emotool3 Compare %file1% %file2%
set /p=Press ENTER to close this window.
