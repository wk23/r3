-- Anub
update creature_template set ScriptName="boss_anubrekhan" where entry=15956;
update creature_template set ScriptName="" where entry=16573;
update creature_template set ScriptName="" where entry=16698;
-- Faerlina
update creature_template set ScriptName="boss_faerlina" where entry=15953;
update creature_template set ScriptName="mob_faerlina_worshipper" where entry=16506;
-- Maexxna
/*update creature_template set ScriptName="boss_maexxna" where entry=15952;
update creature_template set ScriptName="mob_webwrap" where entry=16486;
update creature_template set ScriptName="" where entry=17055;*/

-- For YTDB
update `gameobject_template` set `flags`=5 where `entry` in (181212, 181233,181235,181197,181209);
update `gameobject_template` set `flags`=2 where `entry` in (181126,181195,181167);
-- notice for UDB users: need delete duplicate doors, 'cause instance will become unreacheble. Go to correct door, look it's guid and delete all other doors except this one!
delete from `gameobject` where `guid` in (26367,26369,26370) and `id` = 181167;