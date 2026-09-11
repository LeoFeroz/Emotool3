@echo off

:: This converts between binary files (2ry, emo, emb, emm) and xml files, and back
:: Emotool3 is able to read xml files as if they were binary files, and all other .bat can also use xml files as source!

emotool3 Xmlize %1
set /p=Press ENTER to close this window.
