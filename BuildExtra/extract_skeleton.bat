@echo off

set as_xml=true

emotool3 ExtractSkeleton %1 %as_xml%
set /p=Press ENTER to close this window.

