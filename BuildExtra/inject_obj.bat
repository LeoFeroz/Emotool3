@echo off

:: Wether to set the texture coordinates
:: This probably won't work properly atm (unless it is unchanged from the original model), but it doesn't harm to test
set do_uv=true

:: wether to set vertex normals
set do_normal=true

:: If both do_uv and do_normal are set to false, only gemoetric vertex wll be injected and it should behave like piecemontee inject feature.

emotool3 InjectObj %1 %do_uv% %do_normal%
set /p=Press ENTER to close this window.

