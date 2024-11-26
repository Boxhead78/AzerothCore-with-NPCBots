-- Make sure no alliance bots spawn in horde starting zones and no horde bots spawn in alliance starting zones
-- Durotar
UPDATE `creature_template_npcbot_wander_nodes`
SET `flags` = 5
WHERE `flags` = 1
AND `zoneid` = 14;

-- Barrens
UPDATE `creature_template_npcbot_wander_nodes`
SET `flags` = 5
WHERE `flags` = 1
AND `zoneid` = 17;

-- Mulgore
UPDATE `creature_template_npcbot_wander_nodes`
SET `flags` = 5
WHERE `flags` = 1
AND `zoneid` = 215;

-- Tirisfal
UPDATE `creature_template_npcbot_wander_nodes`
SET `flags` = 5
WHERE `flags` = 1
AND `zoneid` = 85;

-- Eversong Woods
UPDATE `creature_template_npcbot_wander_nodes`
SET `flags` = 5
WHERE `flags` = 1
AND `zoneid` = 3430;

-- Ghostlands
UPDATE `creature_template_npcbot_wander_nodes`
SET `flags` = 5
WHERE `flags` = 1
AND `zoneid` = 3433;

-- Teldrassil
UPDATE `creature_template_npcbot_wander_nodes`
SET `flags` = 3
WHERE `flags` = 1
AND `zoneid` = 141;

-- Azuremyst
UPDATE `creature_template_npcbot_wander_nodes`
SET `flags` = 3
WHERE `flags` = 1
AND `zoneid` = 3524;

-- Bloodmyst
UPDATE `creature_template_npcbot_wander_nodes`
SET `flags` = 3
WHERE `flags` = 1
AND `zoneid` = 3525;

-- Duskwood
UPDATE `creature_template_npcbot_wander_nodes`
SET `flags` = 3
WHERE `flags` = 1
AND `zoneid` = 10;

-- Elwynn Forest
UPDATE `creature_template_npcbot_wander_nodes`
SET `flags` = 3
WHERE `flags` = 1
AND `zoneid` = 12;

-- Westfall
UPDATE `creature_template_npcbot_wander_nodes`
SET `flags` = 3
WHERE `flags` = 1
AND `zoneid` = 40;

-- Dun Morogh
UPDATE `creature_template_npcbot_wander_nodes`
SET `flags` = 3
WHERE `flags` = 1
AND `zoneid` = 1;

-- Loch Modan
UPDATE `creature_template_npcbot_wander_nodes`
SET `flags` = 3
WHERE `flags` = 1
AND `zoneid` = 38;

DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25147;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25147, 'BHBLS001', 0, 4, 5077, 43, 54, 0, -11287.5, -3690.74, 2.7509, 5.32289, '850:0 25148:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25148;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25148, 'BHBLS002', 0, 4, 5077, 43, 54, 0, -11583.6, -3682.66, 5.3235, 1.22443, '25147:0 25149:0 853:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25149;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25149, 'BHBLS003', 0, 4, 4, 43, 54, 0, -11780.1, -3835.2, 56.8799, 3.16839, '25148:0 25150:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25150;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25150, 'BHBLS004', 0, 4, 4, 43, 54, 0, -11967.2, -3829.73, 55.5495, 2.38691, '25151:0 25149:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25151;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25151, 'BHBLS005', 0, 4, 5078, 43, 54, 0, -12153.9, -3765.35, 5.68057, 2.63431, '25150:0 25152:0 25153:0 25164:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25152;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25152, 'BHBLS006', 0, 4, 5078, 43, 54, 0, -12369.8, -3767.69, 5.58381, 1.05703, '25151:0 25153:0 25154:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25153;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25153, 'BHBLS007', 0, 4, 5078, 43, 54, 0, -12355.7, -3522.55, 19.1542, 1.42224, '25151:0 25152:0 25154:0 25164:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25154;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25154, 'BHBLS008', 0, 4, 5078, 43, 54, 0, -12539, -3253.59, 6.7694, 1.3216, '25152:0 25153:0 25155:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25155;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25155, 'BHBLS009', 0, 4, 5083, 43, 54, 5, -12343.9, -3018.18, 88.2169, 2.67248, '25157:0 25154:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25156;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25156, 'BHBLS010', 0, 4, 5084, 43, 54, 3, -12712.3, -2870.17, 1.39316, 5.51562, '25157:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25157;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25157, 'BHBLS011', 0, 4, 4, 43, 54, 0, -12513.3, -2969.24, 1.39229, 3.26151, '25155:0 25156:0 25159:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25158;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25158, 'BHBLS012', 0, 4, 5044, 43, 54, 0, -12452.5, -2408.76, 1.88684, 5.72767, '25159:0 25160:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25159;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25159, 'BHBLS013', 0, 4, 5044, 43, 54, 0, -12256.5, -2539.79, 27.1031, 4.82445, '25157:0 857:0 25158:0 25160:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25160;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25160, 'BHBLS014', 0, 4, 5044, 43, 54, 0, -12146.7, -2405.1, 20.5209, 2.34258, '857:0 25158:0 25159:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25161;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25161, 'BHBLS015', 0, 4, 73, 43, 54, 0, -11696.3, -2388.26, -1.23825, 6.03456, '857:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25162;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25162, 'BHBLS016', 0, 4, 2517, 43, 54, 0, -11229.1, -2893.49, 7.11347, 6.03867, '196:0 835:0 839:0 840:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25163;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25163, 'BHBLS017', 0, 4, 4, 43, 54, 0, -11464.3, -2613.88, 0.953681, 3.88612, '840:0 842:0 843:0 844:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25164;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25164, 'BHBLS018', 0, 4, 5082, 43, 54, 0, -12157.3, -3446.14, 26.4671, 4.17901, '25153:0 25151:0 25165:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25165;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25165, 'BHBLS019', 0, 4, 4, 43, 54, 0, -12046.7, -3406.43, -5.27942, 5.68014, '25164:0 ');
