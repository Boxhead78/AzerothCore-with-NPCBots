DELETE FROM `gossip_menu_option_locale` WHERE `MenuID`=200000 AND `OptionID`=0 AND `Locale`='deDE';
INSERT INTO `gossip_menu_option_locale` (`MenuID`, `OptionID`, `Locale`, `OptionText`, `BoxText`) VALUES (200000, 0, 'deDE', 'Ich möchte in den Künsten der Druiden ausgebildet werden.', '');
DELETE FROM `gossip_menu_option_locale` WHERE `MenuID`=200000 AND `OptionID`=1 AND `Locale`='deDE';
INSERT INTO `gossip_menu_option_locale` (`MenuID`, `OptionID`, `Locale`, `OptionText`, `BoxText`) VALUES (200000, 1, 'deDE', 'Ich möchte meine Talente verlernen.', '');
DELETE FROM `gossip_menu_option_locale` WHERE `MenuID`=200000 AND `OptionID`=2 AND `Locale`='deDE';
INSERT INTO `gossip_menu_option_locale` (`MenuID`, `OptionID`, `Locale`, `OptionText`, `BoxText`) VALUES (200000, 2, 'deDE', 'Informationen zur dualen Talentspezialisierung.', '');
