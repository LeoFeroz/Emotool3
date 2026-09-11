@echo off

:: This function converts a PS3 file to pc format
:: For emo, emm and 2ry, this just swaps the endianess
:: For emb, it adds a dds header to textures

emotool3 Ps3ToPC %1
set /p=Press ENTER to close this window.
