@echo off

::
:: This bat file generates Athena ema with Hilda guard
::

:: The base ema is the ema that... will be used as base :p 

:: The base here is ATG.ema.
set base="ATG.ema"


:: The other is... the other animation, the one that have things we want to copy

:: The other here is HLD.ema
set other="HLD.ema"

:: The result is where the result will be saved.
set result="ATG_eternity_noshield.ema"

::
:: These are the animations (in "other") that we want to copy.
:: You can specify names, or you can specify ids (starting with 0x)
:: Note that because in some ema, several animations can have same name, if you reference animations by name, only the first match will be changed.
:: For those kind of ema, I recommend to use ids instead of names!
::
:: In this case, we want to copy all the GRD animations from Hilda
set animations_src= "HLD_GRD_KAMAE_A,HLD_GRD_KAMAE_B,HLD_GRD_KAMAE_C,HLD_GRD_L,HLD_GRD_H,HLD_GRD_FUTOBI,HLD_GRD_CLASH"

::
:: These are the animations (in "base") that will be replaced by the animations before.
:: The number of items here must match that of animations_src.
::

:: In this case, we specify the atg guards that have almost similar name to that of Hilda.
set animations_dst= "ATG_GRD_KAMAE_A,ATG_GRD_KAMAE_B,ATG_GRD_KAMAE_C,ATG_GRD_L,ATG_GRD_H,ATG_GRD_FUTOBI,ATG_GRD_CLASH"




emotool3 MixAnims %base% %other% %result% %animations_dst% %animations_src%
set /p=Press ENTER to close this window.
