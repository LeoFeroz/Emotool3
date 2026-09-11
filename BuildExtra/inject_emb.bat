@echo off

:: Recursivity: turns on injection of emb inside emb (inside emb... inside emb..., etc)
set recursive=true

:: When an emb has file names, emotool3 injects them by name, otherwise, by index.
:: By setting this param to true, you can make emotool3 ignore names and inject always by index.
set force_index_names=false

emotool3 InjectEmb %1 %recursive% %force_index_names%
set /p=Press ENTER to close this window.

