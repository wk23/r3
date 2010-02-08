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

/* ScriptData
SDName: Boss_Gluth
SD%Complete: 70
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    EMOTE_ZOMBIE                    = -1533119,

    SPELL_MORTALWOUND               = 25646,
    SPELL_DECIMATE                  = 28374,
    SPELL_ENRAGE                    = 28371,
    SPELL_ENRAGE_H                  = 54427,
    SPELL_BERSERK                   = 26662,

    NPC_ZOMBIE_CHOW                 = 16360,
    SPELL_INFECTED_WOUND            = 29306,
};

#define ADD_1X 3269.590
#define ADD_1Y -3161.287
#define ADD_1Z 297.423

#define ADD_2X 3252.157
#define ADD_2Y -3132.135
#define ADD_2Z 297.423

#define ADD_3X 3307.873
#define ADD_3Y -3186.122
#define ADD_3Z 297.423

struct MANGOS_DLL_DECL mob_zombie_chowsAI : public ScriptedAI
{
    mob_zombie_chowsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    bool bIsForceMove;

    void Reset()
    {
        m_creature->CastSpell(m_creature,29307,true);
        bIsForceMove = false;
    }
    void JustDied(Unit* Killer) {}

    /*void DoMeleeAttackIfReady()
    {
        //If we are within range melee the target
        if (m_creature->IsWithinDistInMap(m_creature->getVictim(), ATTACK_DISTANCE))
        {
            //Make sure our attack is ready and we aren't currently casting
            if (m_creature->isAttackReady() && !m_creature->IsNonMeleeSpellCasted(false))
            {
                DoCast(m_creature->getVictim(), SPELL_INFECTED_WOUND, true);
                m_creature->AttackerStateUpdate(m_creature->getVictim());
                m_creature->resetAttackTimer();
            }
        }
    }*/

    void UpdateAI(const uint32 diff)
    {
        if (bIsForceMove)
            m_creature->SetSpeed(MOVE_RUN,0.5f);
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || bIsForceMove)
            return;

        if (!(m_creature->HasAura(29307)))
            DoCast(m_creature,29307,true);

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_gluthAI : public ScriptedAI
{
    boss_gluthAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    std::list<uint64> m_lZombieGUIDList;

    uint32 MortalWound_Timer;
    uint32 Decimate_Timer;
    uint32 Enrage_Timer;
    uint32 Summon_Timer;
    uint32 m_uiBerserkTimer;
    uint32 RangeCheck_Timer;

    void Reset()
    {
        MortalWound_Timer = 8000;
        Decimate_Timer = 105000;
        Enrage_Timer = 60000;
        Summon_Timer = 10000;
        RangeCheck_Timer = 1000;

        m_uiBerserkTimer = MINUTE*8*IN_MILISECONDS;
        m_lZombieGUIDList.clear();

        if (m_pInstance)
            m_pInstance->SetData(ENCOUNT_GLUTH, NOT_STARTED);
    }

    void JustDied(Unit* Killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(ENCOUNT_GLUTH, DONE);
    }

    void Aggro(Unit *who)
    {
        if (m_pInstance)
            m_pInstance->SetData(ENCOUNT_GLUTH, IN_PROGRESS);
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->SetSpeed(MOVE_RUN, 1.2f);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetPositionX() > 3343)
            EnterEvadeMode();

        //MortalWound_Timer
        if (MortalWound_Timer < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_MORTALWOUND);
            MortalWound_Timer = 10000;
        }else MortalWound_Timer -= diff;

        //Decimate_Timer
        if (Decimate_Timer < diff)
        {
            DoCast(m_creature,SPELL_DECIMATE,true); // need core support

            // workaround below
            ThreatList const& t_list = m_creature->getThreatManager().getThreatList();
            if (t_list.size())
            {
                //begin + 1 , so we don't target the one with the highest threat
                ThreatList::const_iterator itr = t_list.begin();
                //std::advance(itr, 1);
                for(; itr!= t_list.end(); ++itr)
                {
                    Unit *target = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());
                    if (target && target->isAlive() && target->GetTypeId() == TYPEID_PLAYER &&
                        (target->GetHealth() > target->GetMaxHealth() * 0.05))
                        target->SetHealth(target->GetMaxHealth() * 0.05);
                }
            }
            // Move Zombies
            if (!m_lZombieGUIDList.empty())
            {
                for(std::list<uint64>::iterator itr = m_lZombieGUIDList.begin(); itr != m_lZombieGUIDList.end(); ++itr)
                    if (Creature* pTemp = (Creature*)Unit::GetUnit(*m_creature, *itr))
                        if (pTemp->isAlive())
                        {
                            ((mob_zombie_chowsAI*)pTemp->AI())->bIsForceMove = true;
                            if (m_creature->GetHealth() > m_creature->GetMaxHealth() * 0.05) // remove when SPELL_DECIMATE is working
                                pTemp->SetHealth(pTemp->GetMaxHealth() * 0.02);
                            pTemp->AddThreat(m_creature, 1000000000.0f); // force move toward to Gluth
                        }
            }
            Decimate_Timer = (m_bIsRegularMode ? 120000 : 105000);
        }else Decimate_Timer -= diff;

        //Enrage_Timer
        if (Enrage_Timer < diff)
        {
            DoCast(m_creature, !m_bIsRegularMode ? SPELL_ENRAGE_H : SPELL_ENRAGE);
            Enrage_Timer = 60000;
        }else Enrage_Timer -= diff;

        if (RangeCheck_Timer < diff)
        {
            if (Creature* zombie = GetClosestCreatureWithEntry(m_creature,NPC_ZOMBIE_CHOW,ATTACK_DISTANCE))
            {
                if (zombie->isAlive())
                {
                    // can infinity cycled on dead zombie, cheat =)
                    DoScriptText(EMOTE_ZOMBIE,m_creature);
                    m_creature->SetHealth(m_creature->GetHealth() + m_creature->GetMaxHealth() * 0.05);
                    zombie->ForcedDespawn();
                }
            }
            RangeCheck_Timer = 1000;
        }else RangeCheck_Timer -= diff;

        //Summon_Timer
        if (Summon_Timer < diff)
        {
            if (m_bIsRegularMode)
            {
                if (Creature* pZombie = m_creature->SummonCreature(NPC_ZOMBIE_CHOW,ADD_1X,ADD_1Y,ADD_1Z,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000))
                {
                    if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                    {
                        pZombie->AI()->AttackStart(pTarget);
                        m_lZombieGUIDList.push_back(pZombie->GetGUID());
                    }
                }

                if (Creature* pZombie = m_creature->SummonCreature(NPC_ZOMBIE_CHOW,ADD_2X,ADD_2Y,ADD_2Z,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000))
                {
                    if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                    {
                        pZombie->AI()->AttackStart(pTarget);
                        m_lZombieGUIDList.push_back(pZombie->GetGUID());
                    }
                }

                if (Creature* pZombie = m_creature->SummonCreature(NPC_ZOMBIE_CHOW,ADD_3X,ADD_3Y,ADD_3Z,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000))
                {
                    if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                    {
                        pZombie->AI()->AttackStart(pTarget);
                        m_lZombieGUIDList.push_back(pZombie->GetGUID());
                    }
                }
            }
            else
            {
                if (Creature* pZombie = m_creature->SummonCreature(NPC_ZOMBIE_CHOW,ADD_1X,ADD_1Y,ADD_1Z,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000))
                {
                    if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                    {
                        pZombie->AI()->AttackStart(pTarget);
                        m_lZombieGUIDList.push_back(pZombie->GetGUID());
                    }
                }
            }
            Summon_Timer = 10000;
        } else Summon_Timer -= diff;

        //m_uiBerserkTimer
        if (m_uiBerserkTimer < diff)
        {
            DoCast(m_creature, SPELL_BERSERK, true);
            m_uiBerserkTimer = MINUTE*5*IN_MILISECONDS;
        }else m_uiBerserkTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gluth(Creature* pCreature)
{
    return new boss_gluthAI(pCreature);
}

CreatureAI* GetAI_mob_zombie_chows(Creature* pCreature)
{
    return new mob_zombie_chowsAI(pCreature);
}

void AddSC_boss_gluth()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_gluth";
    newscript->GetAI = &GetAI_boss_gluth;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_zombie_chows";
    newscript->GetAI = &GetAI_mob_zombie_chows;
    newscript->RegisterSelf();
}