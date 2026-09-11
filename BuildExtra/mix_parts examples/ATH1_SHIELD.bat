@echo off

::
:: This bat file generates "Athena with clothes 1 + shield", alias "ATH1_SHIELD"
::

:: The base is the character that... will be used as base :p 
:: Which you will usually make the one that has more parts of the model you want to create, 
:: unless you are masochist.
:: All files, emo, emb, emm, and 2ry must exist.
::
:: The base here is obviously Athena with clothes 1.
set base="ATN_00"

:: The other is... the other character for the mix, obvious thing is obvious.
:: Files emo, emb and emm must exist. 2ry is not neccesary, at least for now.
::
:: The other here is armored Athena, from which we will take the shield.
set other="ATG_00"

:: The result is where the resulting files will go.
:: Although, you could use directly the final files names of your destination,
:: I prefer to use descriptive names, because sometimes the destination names collides with
:: the base or with other, so I just put a name without collision, and then just rename it when 
:: it is succesfull.
:: The files here don't need to exist; if they do, they will be overwritten without warning.
:: 4 files will be generated, emo, emb, emm and 2ry (the 2ry is mrely a copy of the base 2ry)
::
:: I'll just name it like that.
set result="ATH1_SHIELD"


:: From here, most params can take multiple strings, separated with ","
:: It is important that you don't leave any blank space after ",".
:: Also note that the program is case-sensitive when reading part names, bone names, etc
:: although that may change in the future if I find out that the game engine is not case-sensitive, 
:: which I currently don't know.
:: When a param is not necessary, use "NULL".


:: Here go the parts that we do NOT want from the base.
::
:: In this case, since we don't want to remove anything from Athena with clothes, just use NULL
set parts_to_remove="NULL"

:: These are the parts of "other" that we want in the final model.
::
:: In this case, we want the shield, and only the shield.
set parts_to_add="Hide_0,Hidemodeledge_0"


:: The following two params are connected. parts_to_rename_from indicates the names of 
:: *existing* parts in the base that we want to rename. parts_to_rename_to are the new
:: names. Obvously they must contain the same number of strings, or program will complain.
:: In case you are wondering when to use these, usually you want to rename a part, when
:: a part of the base that you want to keep has the same name that a part of other that you 
:: want to add, to avoid name collision.
::
:: In this case, Athena weapon in the clothed Athena has the same name as the shield of armored Athena.
:: Not only we want to avoid the collision, but "Hide_0" is a special name used for parts that get hidden
:: in battle.
:: So will be renaming this to modelOPT/modelOPTedge, since those are the names used in Armored Athena.
set parts_to_rename_from="Hide_0,Hidemodeledge_0"
set parts_to_rename_to="modelOPT,modelOPTedge"

:: Sometimes, we don't want the program to add some bones from other to the result model.
:: The more complex the mod, the more probabilities of needing this param.
::
:: This mod is simple enough to not need this.
set bones_to_not_add="NULL"

:: This will exclude the program from trying to add bones linked to these parts.
:: To be clear: here you specify part names, not bone names, and the program will just skip
:: trying to add the bones linked to those parts.
:: This is used to avoid the bones_to_not_add being too big in complex cases, when you know
:: that theere is a big similarity in the part being added with the part being replaced.
:: Misusing this param will make the program unable to find some bones when linking new parts.
::
:: This is a very simple mod. No need for this.
set exclude_bones_parts="NULL"

:: And this... well, sometimes you need to add more bones than initially the program can guess.
:: The more complex the mod, the more probabilities of needing this.
:: This param can take two forms: the easy one, keyword "GUESS", will make the program use an
:: algorithm to find out this param. (Note, the use of "GUESS" algorithm may make you need to add
:: some additional bone later to bones_to_not_add)
::
:: The hard one: if GUESS algorithm of the program is not enough, you can use this as a list of 
:: bones separated by ",". How to guess those bones... well... urgh, there is not an exact procedure,
:: which is why I said "hard one".
:: And of course, it can be set to "NULL" when not needed at all.
::
:: We don't need this one, either.
set additional_bones="NULL"

::
:: Those are all the params for now. The following params are reserved params, that may be used in the future.
:: Setting them to NULL "ensures" that they will be ignored for future versions if they get an use, where "ensures"
:: means if I don't screw it.
::
set reserved_param1="NULL"
set reserved_param2="NULL"
set reserved_param3="NULL"
set reserved_param4="NULL"
set reserved_param5="NULL"
set reserved_param6="NULL"
set reserved_param7="NULL"
set reserved_param8="NULL"
set reserved_param9="NULL"
set reserved_param10="NULL"
set reserved_param11="NULL"
set reserved_param12="NULL"
set reserved_param13="NULL"
set reserved_param14="NULL"
set reserved_param15="NULL"
set reserved_param16="NULL"


:: Command time
emotool3 MixParts %base% %other% %result% %parts_to_remove% %parts_to_add% %parts_to_rename_from% %parts_to_rename_to% %bones_to_not_add% %exclude_bones_parts% %additional_bones% %reserved_param1% %reserved_param2% %reserved_param3% %reserved_param4% %reserved_param5% %reserved_param6% %reserved_param7% %reserved_param8% %reserved_param9% %reserved_param10% %reserved_param11% %reserved_param12% %reserved_param13% %reserved_param14% %reserved_param15% %reserved_param16%
set /p=Press ENTER to close this window.
