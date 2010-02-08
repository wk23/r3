/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "precompiled.h"
#include "naxxramas.h"


/*Poison Cloud 26590
Slime Spray 28157
Fallout slime 28218
Mutating Injection 28169
Mutagen Explosion 28206
Enrages 26527
update creature_template set ScriptName='boss_grobbulus' where entry=15931;
update creature_template set ScriptName='npc_grobbulus_poison_cloud',faction_A=14,faction_H=14,unit_flags=unit_flags|'33554432' where entry=16363;
update creature_template set minhealth=75600,maxhealth=75600 where entry=16290;*/

#define SPELL_BOMBARD_SLIME         28280

#define SPELL_POISON_CLOUD          28240
#define SPELL_MUTATING_INJECTION    28169
#define SPELL_SLIME_SPRAY           28157
#define H_SPELL_SLIME_SPRAY         54364
#define SPELL_BERSERK               26662

#define MOB_FALLOUT_SLIME   16290
//hack

struct MANGOS_DLL_DECL boss_grobbulusAI : public ScriptedAI
{
    boss_grobbulusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 PoisonCloud_Timer;
    uint32 MutatingInjection_Timer;
    uint32 SlimeSpary_Timer;
    uint32 Enrage_Timer;
    //	uint32 PoisonGuard;
    void Reset()
    {
        PoisonCloud_Timer = 15000;
        MutatingInjection_Timer = 20000;
        SlimeSpary_Timer = 15000+rand()%15000;
        Enrage_Timer = 300000;


        if (m_pInstance)
            m_pInstance->SetData(ENCOUNT_GROBBULUS, NOT_STARTED);
    }

    void JustDied(Unit* Killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(ENCOUNT_GROBBULUS, DONE);
    }

    void Aggro(Unit *who)
    {
        if (m_pInstance)
            m_pInstance->SetData(ENCOUNT_GROBBULUS, IN_PROGRESS);
    }

    void SpellHitTarget(Unit *target, const SpellEntry *spell)

    {
        if(spell->Id == SPELL_SLIME_SPRAY || spell->Id == H_SPELL_SLIME_SPRAY && target->GetTypeId() == TYPEID_PLAYER)
        {
            if (Creature* pSlime = m_creature->SummonCreature(MOB_FALLOUT_SLIME, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, MINUTE*1*IN_MILISECONDS))
                pSlime->SetInCombatWithZone();
        }
        /*//mob poison slime
        if(spell->Id == SPELL_POISON_CLOUD)
        {
            if (Creature* pSlime = m_creature->SummonCreature(MOB_POISON_SLIME, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, MINUTE*1.5*IN_MILISECONDS))
                pSlime->SetInCombatWithZone();
        }*/
    } 

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (PoisonCloud_Timer < diff)
        {
            DoCast(m_creature, SPELL_POISON_CLOUD);
            PoisonCloud_Timer = 15000;
        }else PoisonCloud_Timer -= diff;

        if (MutatingInjection_Timer < diff)
        {
            // small hack to avoid target non players
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,1))
            {
                if (target->GetTypeId() == TYPEID_PLAYER)
                    DoCast(target, SPELL_MUTATING_INJECTION);
                else if (target = SelectUnit(SELECT_TARGET_RANDOM,1))
                    DoCast(target, SPELL_MUTATING_INJECTION);
            }

            if (m_creature->GetHealth() < m_creature->GetMaxHealth() * 0.75f)
                MutatingInjection_Timer = 15000;
            else if (m_creature->GetHealth() < m_creature->GetMaxHealth() * 0.50f)
                MutatingInjection_Timer = 10000;
            else if (m_creature->GetHealth() < m_creature->GetMaxHealth() * 0.25f)
                MutatingInjection_Timer = 6000; // DIE DIE ALL!!!!
            else
                MutatingInjection_Timer = 20000;            
        }else MutatingInjection_Timer -= diff;

        if (SlimeSpary_Timer < diff)
        {
            DoCast(m_creature, m_bIsRegularMode ? H_SPELL_SLIME_SPRAY : SPELL_SLIME_SPRAY);
            SlimeSpary_Timer = 15000+rand()%15000;
        }else SlimeSpary_Timer -= diff;

        if (Enrage_Timer < diff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            Enrage_Timer = 10000;
        }else Enrage_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_grobbulus_poison_cloudAI : public Scripted_NoMovementAI
{
    npc_grobbulus_poison_cloudAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        Reset();
    }

    uint32 Cloud_Timer;
    uint32 Despawn_Timer;

    void Reset()
    {
        Cloud_Timer = 1000;
        Despawn_Timer = 75000;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        //m_creature->DeleteThreatList;
        //return;

    }
    void Aggro(Unit *who)
    {
        return;
    }

    void UpdateAI(const uint32 diff)
    {
        if (Cloud_Timer < diff)
        {
            DoCast(m_creature, 54362);
            Cloud_Timer = 120000;
            WorldPacket data(SMSG_PLAY_SPELL_VISUAL,8+4);
            data << m_creature->GetGUID();
            data << 7020;
            m_creature->SendMessageToSet(&data,false);
        }else Cloud_Timer -= diff;

        if (Despawn_Timer < diff)
            m_creature->ForcedDespawn();
        else Despawn_Timer -=diff;

        //m_creature->RemoveAura(15576, NULL);
    }
};

CreatureAI* GetAI_boss_grobbulus(Creature* pCreature)
{
    return new boss_grobbulusAI(pCreature);
}

CreatureAI* GetAI_npc_grobbulus_poison_cloud(Creature* pCreature)
{
    return new npc_grobbulus_poison_cloudAI(pCreature);
}

void AddSC_boss_grobbulus()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_grobbulus";
    newscript->GetAI = &GetAI_boss_grobbulus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_grobbulus_poison_cloud";
    newscript->GetAI = &GetAI_npc_grobbulus_poison_cloud;
    newscript->RegisterSelf();
}
