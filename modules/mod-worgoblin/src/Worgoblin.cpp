#include "worgoblin_loader.h"
#include "Chat.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "Config.h"

class announce : public PlayerScript {

public:
    announce() : PlayerScript("announce") { }

    void OnLogin(Player* player) override
    {
        if (sConfigMgr->GetOption<bool>("Announce.enable", true))
        {
            ChatHandler(player->GetSession()).SendSysMessage("This server is running the Worgoblin module."); // Hell yea
        }
    }
};

class playerscript_spell_two_forms : public PlayerScript
{
public:
    playerscript_spell_two_forms() : PlayerScript("playerscript_spell_two_forms") { }

    uint32 _updateTimer = 0;

    void OnPlayerJustDied(Player* player) override
    {
        if (!player)
            return;

        if (player->GetDisplayId() == 29317 || player->GetDisplayId() == 30217)
        {
            player->CastSpell(player, 98598, true);
        }
    }

    void OnUpdate(Player* player, uint32 p_time) override
    {
        if (!player)
            return;

        _updateTimer += p_time;
        if (_updateTimer < 1000)
            return;

        _updateTimer = 0;
        if (player->IsInCombat() && (player->GetDisplayId() == 29317 || player->GetDisplayId() == 30217))
        {
            player->CastSpell(player, 98598, true);
        }
    }
};

class spell_rocket_barrage : public SpellScript
{
    PrepareSpellScript(spell_rocket_barrage);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        int32 basePoints = 0 + caster->GetLevel() * 2;
        basePoints += caster->SpellBaseDamageBonusDone(GetSpellInfo()->GetSchoolMask()) * 0.429; //BM=0.429 here, don't ask me how.
        basePoints += caster->GetTotalAttackPowerValue(caster->getClass() != CLASS_HUNTER ? BASE_ATTACK : RANGED_ATTACK) * 0.25; // 0.25=BonusCoefficient, hardcoding it here
        SetEffectValue(basePoints);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_rocket_barrage::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

void Add_Worgoblin()
{
    new announce();
    new playerscript_spell_two_forms();
    RegisterSpellScript(spell_rocket_barrage);
}
