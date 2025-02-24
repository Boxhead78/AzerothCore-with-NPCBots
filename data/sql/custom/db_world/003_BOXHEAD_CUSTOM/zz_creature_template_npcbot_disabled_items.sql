-- Cataclysm Dev Items
INSERT IGNORE INTO acore_world.creature_template_npcbot_disabled_items (id)
SELECT entry
FROM acore_world.item_template
WHERE VerifiedBuild = 16000;
