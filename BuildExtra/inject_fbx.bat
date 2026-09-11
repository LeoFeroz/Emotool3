@echo off

:: INJECT parameters section

:: If false, emotool3 will use the fbx sdk to generate tangents data, which is required by some parts of some emo files.
:: Otherwise, the existing data in the fbx is used. 
:: This parameter is best set to false when the fbx file has been exported by a 3d editor, and true when you are using files exported by emotool3 itself. 
set USE_FBX_TANGENT=false


:: Parameters for both, EXPORT and INJECT
:: Any parameter in this section must be set to the same in export_fbx.bat and ibject_fbx.bat!

:: Values: ALL, everything is injected. NORMAL: only normal parts are injected. EDGE: only edge parts are injected.
set MODE=ALL

emotool3 InjectFbx %1 %USE_FBX_TANGENT% %MODE%
set /p=Press ENTER to close this window.
