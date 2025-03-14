UPDATE `gameobject_template` SET `AIName`="" WHERE `entry` IN (164825);

DELETE FROM `creature_loot_template` WHERE `Entry`=11583 AND `Item`=30486 AND `Reference`=30486 AND `GroupId`=0;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES (11583, 30486, 30486, 100, 0, 1, 0, 2, 2, NULL);
DELETE FROM `creature_loot_template` WHERE `Entry`=21212 AND `Item`=34062 AND `Reference`=34062 AND `GroupId`=3;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES (21212, 34062, 34062, 100, 0, 1, 3, 2, 2, 'Lady Vashj - (ReferenceTable)');
DELETE FROM `creature_loot_template` WHERE `Entry`=21212 AND `Item`=90062 AND `Reference`=34062 AND `GroupId`=4;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES (21212, 90062, 34062, 100, 0, 1, 4, 2, 2, 'Lady Vashj - (ReferenceTable)');
DELETE FROM `creature_loot_template` WHERE `Entry`=21214 AND `Item`=34060 AND `Reference`=34060 AND `GroupId`=0;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES (21214, 34060, 34060, 100, 0, 1, 0, 2, 2, 'Fathom-Lord Karathress - (ReferenceTable)');
DELETE FROM `creature_loot_template` WHERE `Entry`=21215 AND `Item`=34059 AND `Reference`=34059 AND `GroupId`=0;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES (21215, 34059, 34059, 100, 0, 1, 0, 2, 2, 'Leotheras the Blind - (ReferenceTable)');
