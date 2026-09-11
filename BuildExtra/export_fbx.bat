@echo off

:: EXPORT parameters section

:: It can be set to 2010, 2011, 2012, 2013, 2014 and 2016 for compatibility with different Autodesk products.
set VERSION=2013

:: If true, it will generate an ascii fbx file, when false it will generate a binary file.
:: Ascii files have different compatibility between programs... sometimes for better, sometimes for worse.
set ASCII=false

:: Set the up axis for the file. Valid values are Z and Y.
set UP_AXIS=Z

:: Sets the unit. Valid values are m (for meters) and i (for inches)
set UNIT=m

:: Parameters for both, EXPORT and INJECT
:: Any parameter in this section must be set to the same in export_fbx.bat and ibject_fbx.bat!

:: Values: ALL, everything is exported. NORMAL: only normal parts are exported. EDGE: only edge parts are exported.
set MODE=ALL

emotool3 ExportFbx %1 %VERSION% %ASCII% %UP_AXIS% %UNIT% %MODE%
set /p=Press ENTER to close this window.
