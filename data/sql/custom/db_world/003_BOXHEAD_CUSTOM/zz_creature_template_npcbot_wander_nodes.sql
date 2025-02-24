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
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25166;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25166, 'BHBRS001', 1, 17, 4844, 8, 21, 5, -812.541, -1574.6, 140.36, 3.22102, '25167:0 25172:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25167;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25167, 'BHBRS002', 1, 17, 4844, 8, 21, 0, -787.244, -1569.27, 140.549, 0.182154, '25166:0 25168:0 25172:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25168;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25168, 'BHBRS003', 1, 17, 4985, 8, 21, 0, -643.844, -1416.76, 154.126, 0.903747, '25167:0 25169:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25169;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25169, 'BHBRS004', 1, 17, 4985, 8, 21, 0, -427.418, -1160.88, 176.665, 1.09648, '25170:0 25168:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25170;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25170, 'BHBRS005', 1, 17, 4985, 8, 21, 0, -437.709, -948.129, 90.4607, 0.820613, '25171:0 1619:0 25169:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25171;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25171, 'BHBRS006', 1, 406, 469, 8, 21, 0, -279.987, -728.885, 37.2641, 1.35784, '1620:0 1621:0 1622:0 25170:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25172;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25172, 'BHBRS007', 1, 17, 17, 8, 21, 0, -947.503, -1656.05, 93.3762, 4.00353, '1557:0 25166:0 25167:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25173;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25173, 'BHTSN001', 1, 15, 511, 33, 45, 0, -4812.29, -3201.11, 31.3739, 3.62048, '233:0 1911:0 25174:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25174;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25174, 'BHTSN002', 1, 15, 511, 33, 45, 0, -5147.12, -3408.45, 201.302, 3.18947, '25173:0 25175:0 25176:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25175;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25175, 'BHTSN003', 1, 400, 400, 25, 35, 5, -5374.95, -3438.05, 88.8843, 4.16301, '25174:0 25176:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25176;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25176, 'BHTSN004', 1, 400, 400, 25, 35, 0, -5381.13, -3643, 86.111, 1.24196, '25174:0 25175:0 25177:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25177;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25177, 'BHTSN005', 1, 400, 400, 25, 35, 0, -5367.13, -3907.76, 86.3665, 4.77374, '25176:0 25178:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25178;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25178, 'BHTSN006', 1, 400, 823, 25, 35, 0, -5347.26, -4364.95, 122.365, 1.96371, '25177:0 25179:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25179;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25179, 'BHTSN007', 1, 400, 823, 25, 35, 0, -5532.77, -4385.7, 86.1506, 4.97572, '25178:0 25180:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25180;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25180, 'BHTSN008', 1, 400, 823, 25, 35, 0, -5767.47, -4425.01, 86.1149, 3.3072, '25179:0 25181:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25181;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25181, 'BHTSN009', 1, 400, 822, 25, 35, 0, -5968.62, -4441.3, 86.1113, 3.7431, '25180:0 25182:0 25183:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25182;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25182, 'BHTSN010', 1, 400, 822, 25, 35, 0, -6116.18, -4469.32, 90.228, 1.65251, '25181:0 25183:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25183;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25183, 'BHTSN011', 1, 400, 822, 25, 35, 0, -6231.95, -4465.51, 86.11, 3.95496, '25181:0 25182:0 25184:0 25185:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25184;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25184, 'BHTSN012', 1, 400, 822, 25, 35, 0, -6316.66, -4514.19, 86.6723, 4.38541, '25183:0 25190:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25185;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25185, 'BHTSN013', 1, 400, 822, 25, 35, 0, -6355.7, -4631.98, 117.256, 3.49942, '25183:0 25186:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25186;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25186, 'BHTSN014', 1, 400, 822, 25, 35, 0, -6284.9, -4617.54, 79.0741, 5.5454, '25185:0 25187:0 25190:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25187;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25187, 'BHTSN015', 1, 400, 822, 25, 35, 0, -6209.6, -4665.24, 67.2256, 4.1042, '25186:0 25188:0 25190:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25188;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25188, 'BHTSN016', 1, 400, 822, 25, 35, 0, -6237.26, -4687.66, 79.7261, 4.01781, '25187:0 25189:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25189;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25189, 'BHTSN017', 1, 400, 822, 25, 35, 0, -6322.76, -4620.49, 84.1553, 1.41578, '25188:0 25190:0 ');
DELETE FROM `creature_template_npcbot_wander_nodes` WHERE `id`=25190;
INSERT INTO `creature_template_npcbot_wander_nodes` (`id`, `name`, `mapid`, `zoneid`, `areaid`, `minlevel`, `maxlevel`, `flags`, `x`, `y`, `z`, `o`, `links`) VALUES (25190, 'BHTSN018', 1, 400, 822, 25, 35, 0, -6313.03, -4585.65, 84.167, 5.28606, '25186:0 25187:0 25189:0 25184:0 ');
