

#include "precompiled.h"

#define SP_CHAIN_LIGHTNING      64213
#define H_SP_CHAIN_LIGHTNING    64215
/* можно использовать 49271 - шамовский чайник, т.к. с "родным" 
спеллом наблюдался баг - Эмалон задевал им своих же аддов и они 
начинали его бить */

#define SP_LIGHTNING_NOVA       64216
#define H_SP_LIGHTNING_NOVA     65279

#define SP_BERSERK              26662

#define SP_STONED               63080

#define SP_SHOCK                64363

#define SP_OVERCHARGE           64218
#define SP_OVERCHARGED          64217
#define SP_OVERCHARGED_BLAST    64219


/****************************************************************
    Tempest Minion AI
****************************************************************/
struct MANGOS_DLL_DECL mob_tempest_minionAI : public ScriptedAI
{
    mob_tempest_minionAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* pInstance;

    bool overcharged;
    uint32 OverchargeTimer;
    uint32 OverchargedStacks;

    uint32 ShockTimer;

    void Reset()
    {
        overcharged = false;
        OverchargeTimer = 2000;
        OverchargedStacks = 0;
        ShockTimer = 8000+rand()%4000;

        m_creature->RemoveAllAuras();
    }

    void Aggro(Unit* who)
    {
        Unit *emalon = Unit::GetUnit((*m_creature), pInstance->GetData64(4));
        if(emalon && emalon->isAlive())
        {
            emalon->AddThreat(who, 1.0f);
        }
    }

    void UpdateAI(const uint32 diff)
    {  
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if(overcharged)
        {
            if(OverchargeTimer < diff)
            {
                if(OverchargedStacks < 10)
                {
                    DoCast(m_creature, SP_OVERCHARGED);
                    OverchargeTimer = 2000;
                }
                else if(OverchargedStacks == 10)
                {
                    DoCast(m_creature->getVictim(), SP_OVERCHARGED_BLAST);
                    OverchargeTimer = 1500;

                }
                else if(OverchargedStacks >= 11)
                {
                    OverchargedStacks = 0;
                    overcharged = false;
                    m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    return;
                }            
                OverchargedStacks++;
            }
            else OverchargeTimer -= diff;
        }

        if(ShockTimer < diff)
        {
            DoCast(m_creature->getVictim(), SP_SHOCK);
            ShockTimer = 8000+rand()%4000;
        }
        else ShockTimer -= diff;

        DoMeleeAttackIfReady();
    }

    void Overcharge()
    {
        overcharged = true;
    }
};


/****************************************************************
    Emalon the Storm Watcher AI
****************************************************************/

struct MANGOS_DLL_DECL boss_emalonAI : public ScriptedAI
{
    boss_emalonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Heroic = m_creature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance* pInstance;
    bool Heroic;

    int overchargedMinion;
    //Timers
    uint32 LightningTimer;
    uint32 LightningCount;
    uint32 NovaTimer;
    uint32 EnrageTimer;
    uint32 OverchargeTimer;

    void Reset()
    {
        EnrageTimer = 360000; //6 ?
        LightningTimer = 10000 + rand()%30000;
        LightningCount = 0;
        NovaTimer = 20000;
        OverchargeTimer = 45000;

        m_creature->RemoveAllAuras();
    }

    void Aggro(Unit* who)
    {
        if(!pInstance) return;
        int i;
        for(i=0; i<4; ++i)
        {
            Unit *minion = Unit::GetUnit((*m_creature), pInstance->GetData64(i));
            if(minion)
            {
                if(minion->isDead())
                    ((Creature *)minion)->Respawn();
                else if(!minion->getVictim())
                    minion->AddThreat(who, 1.0f);
            }
        }
    }

    void JustDied(Unit *killer)
    {
        if(!pInstance) return;
        int i;
        for(i=0; i<4; ++i)
        {
            Unit *minion = Unit::GetUnit((*m_creature), pInstance->GetData64(i));
            if(minion && minion->isAlive())
            {
                minion->DealDamage(minion, minion->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        //смотрим аддов, если кто-то мертв - воскрешаем, если кто-то отдыхает, посылаем в бой
        int i;
        for(i=0; i<4; i++)
        {
            Unit *minion = Unit::GetUnit((*m_creature), pInstance->GetData64(i));
            if(minion)
            {
                if(minion->isDead())
                {
                    ((Creature *)minion)->Respawn();
                    minion->MonsterTextEmote("appears to defend Emalon!", 0, true);
                }
                else if(!minion->getVictim())
                    minion->AddThreat(m_creature->getVictim(), 1.0f);
            }
        }


        if(OverchargeTimer < diff)
        {
            overchargedMinion = irand(0,3);
            Unit *minion = Unit::GetUnit((*m_creature), pInstance->GetData64(overchargedMinion));
            if(minion && minion->isAlive())
            {
                m_creature->MonsterTextEmote("overcharges Tempest Minion!", 0, true);
                minion->SetHealth(minion->GetMaxHealth());
                //извращенная конструкция, наверное можно сделать как-нить покрасивее все это, но пашет)
                ((mob_tempest_minionAI *)((Creature *) minion)->AI())->Overcharge();
            }
            OverchargeTimer = 45000;
        }
        else OverchargeTimer -= diff;

        if (LightningTimer < diff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
            {
                //в теории надо так, но на практике используем шамовский чайник с 
                //искуственно завышенным уроном
                //DoCast(target, Heroic ? H_SP_CHAIN_LIGHTNING : SP_CHAIN_LIGHTNING);
                int32 dmg = Heroic ? 10000 : 7000;
                m_creature->CastCustomSpell(target, 49271, &dmg, 0, 0, false);
            }
            LightningTimer = 10000 + rand()%30000;
        }
        else LightningTimer -= diff;

        if (NovaTimer < diff)
        {
            DoCast(m_creature->getVictim(), Heroic ? H_SP_LIGHTNING_NOVA : SP_LIGHTNING_NOVA);
            NovaTimer = 45000;
        }
        else NovaTimer -= diff;

        if(EnrageTimer < diff)
        {
            DoCast(m_creature, SP_BERSERK);
            EnrageTimer = 30000;
        }
        else EnrageTimer -= diff;

        DoMeleeAttackIfReady();
    }
};




CreatureAI* GetAI_boss_emalonAI(Creature* pCreature)
{
    return new boss_emalonAI(pCreature);
}

CreatureAI* GetAI_mob_tempest_minionAI(Creature* pCreature)
{
    return new mob_tempest_minionAI(pCreature);
}

void AddSC_boss_emalon()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_emalon";
    newscript->GetAI = &GetAI_boss_emalonAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_tempest_minion";
    newscript->GetAI = &GetAI_mob_tempest_minionAI;
    newscript->RegisterSelf();
}
