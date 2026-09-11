@echo off

:: Recursivity: turns on extraction of emb inside emb (inside emb... inside emb..., etc)
set recursive=true

:: When an emb has file names, emotool3 extracts them by name, otherwise, by index.
:: By setting this param to true, you can make emotool3 ignore names and extract always by index.
:: Remember that if you plan to inject those, you should also modify the similar variable in inject_emb.bat!
set force_index_names=false

emotool3 ExtractEmb %1 %recursive% %force_index_names%
set /p=Press ENTER to close this window.

