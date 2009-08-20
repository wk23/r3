# В базе Готик и некоторые адды без маны, из-за чего не могут кастовать
UPDATE creature_template SET minmana=425800, maxmana=425800 where entry IN (16060, 29955);
UPDATE creature SET curmana=425800 WHERE id=16060;
UPDATE creature_template SET minmana=12774, maxmana=12774 where entry IN (16127, 30264);
UPDATE creature SET curmana=12774 WHERE id=16127;

# Ставим нужные скрипты мобам и боссу
UPDATE creature_template SET ScriptName='boss_gothik' WHERE entry=16060;
UPDATE creature_template SET ScriptName='mob_gothik_trainee' WHERE entry=16124;
UPDATE creature_template SET ScriptName='mob_gothik_dk' WHERE entry=16125;
UPDATE creature_template SET ScriptName='mob_gothik_rider' WHERE entry=16126;
UPDATE creature_template SET ScriptName='mob_gothik_trainee' WHERE entry=16127;
UPDATE creature_template SET ScriptName='mob_gothik_dk' WHERE entry=16148;
UPDATE creature_template SET ScriptName='mob_gothik_rider' WHERE entry=16150;

# Корректировка количества хп некоторых аддов нормал версии
UPDATE creature_template SET minhealth=10800, maxhealth=10800 WHERE entry=16124;
UPDATE creature_template SET minhealth=32582, maxhealth=32582 WHERE entry=16125;
UPDATE creature_template SET minhealth=52132, maxhealth=52132 WHERE entry=16150;
UPDATE creature_template SET minhealth=32582, maxhealth=32582 WHERE entry=16148;