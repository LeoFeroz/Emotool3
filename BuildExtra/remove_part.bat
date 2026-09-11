@echo off

:: Usually you need to remove two objects to actually remove one part group/part or subpart. The normal one and the one with a word "edge" attached.
:: Since emotool3, you can now specify subindexes to remove subparts
:: Example: modelOPT -> it is a parts group (THIS IS WHAT YOU WANT MOST OF THE TIMES!!!!!) (note: most groups except "face", and "edge" have only 1 part inside) 
:: Example: modelOPT_0000 -> it is a part (includes subparts) Note: very few models have parts with more than 1 subpart. Hilda cape+shoulder pads is one of the few examples.
:: Exmaple: modelOPT_0000_0000 -> it is a subpart


:: The example below is to remove the Hilda cape + shoulder pads (they are in same parts group, and same part, but in different subpart, but this command removes whole parts group)
set parts=modelOPT,modelOPTedge

:: The (commented) example below is to remove the Hilda cape without removing shoulder pads 
::set parts=modelOPT_0000_0000,modelOPTedge_0000_0000

emotool3 RemovePart %1 %parts%
set /p=Press ENTER to close this window.
