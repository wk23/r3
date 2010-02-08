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
SDName: Boss_Loatheb
SD%Complete: 90
SDComment: Doors don't work, timing not off, but very close
SDCategory: Naxxramas
EndScriptData */

/*SQL
update creature_template set ScriptName="mob_loatheb_spores" where entry=16286
update creature_template set ScriptName="boss_loatheb" where entry=16011
*/

/*USED
http://www.youtube.com/watch?v=_RboWVb4IFc
http://www.wowhead.com/?npc=16011#comments:0
http://www.noob-club.ru/index.php?page=23#Loatheb
off-players stories
*/
#include "precompiled.h"
#include "naxxramas.h"
//Boss Loatheb spells
#define SPELL_NECROTIC_AURA         55593
#define SPELL_DEATHBLOOM            29865
#define SPELL_DEATHBLOOM_H          55053
#define SPELL_INEVITABLE_DOOM       29204
#define SPELL_INEVITABLE_DOOM_H     55052

//Mob Loatheb Spore and his spell
#define MOB_LOATHEB_SPORE			16286
#define SPELL_FUNGAL_CREEP          29232

//Loatheb Spore spawn places
#define ADD_1X 2957.040
#define ADD_1Y -3997.590
#define ADD_1Z 274.280

#define ADD_2X 2909.130
#define ADD_2Y -4042.970
#define ADD_2Z 274.280

#define ADD_3X 2861.102
#define ADD_3Y -3997.901
#define ADD_3Z 274.280

struct MANGOS_DLL_DECL boss_loathebAI : public ScriptedAI
{
    boss_loathebAI(Creature *c) : ScriptedAI(c) 
    {
        m_pInstance = ((ScriptedInstance*)c->GetInstanceData());
        m_bIsRegularMode = c->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_ach_10ppl;
    bool m_ach_25ppl;
    uint32 m_count_ppl;
    uint32 Ach_Timer;
    bool ach_spore_10;
    bool ach_spore_25;
    uint32 NecroticAura_Timer;
    uint32 Deathbloom_Timer;
    uint32 InevitableDoom_Timer;
    uint32 InevitableDoom_Cooldown;
    uint32 Summon_Timer;

    void Reset()
    {
        if(m_pInstance)
            m_pInstance->SetData(ENCOUNT_LOATHEB, NOT_STARTED);

        ach_spore_10 = true;
        ach_spore_25 = true;
        m_ach_10ppl = true;
        m_ach_25ppl = true;
        Ach_Timer = 10000;
        m_count_ppl = 0;
        NecroticAura_Timer = 20000;
        Deathbloom_Timer = 30000;
        InevitableDoom_Timer = 120000;
        InevitableDoom_Cooldown = 40000;//This is cooldown for Doom spell. 40000 means 30sec cooldown + 10sec spelltime, so next doom will be cast 30 sec
										//after first ends. cooldown decreases by 5 sec after each doom
        Summon_Timer = 8000;
	}

	void Aggro(Unit *who)
    {
        if(m_pInstance)
            m_pInstance->SetData(ENCOUNT_LOATHEB, IN_PROGRESS);
        CheckAch();
    }

    void KillSpore()
    {
        ach_spore_10 = false;
        ach_spore_25 = false;
    }

    void JustDied(Unit* who)
    {

        if (!m_pInstance)
            return;
        
        m_pInstance->SetData(ENCOUNT_LOATHEB, DONE);
        
        Map::PlayerList const &PlList = m_pInstance->instance->GetPlayers();
        if (PlList.isEmpty())
            return;
        for(Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
        {
            if (Player* pPlayer = i->getSource())
            {
                if (!m_creature->IsWithinDistInMap(pPlayer,200))
                    continue;

                if (m_bIsRegularMode && m_ach_10ppl)
                    pPlayer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE,m_creature->GetEntry(),1,0,0,7155);
                else if (!m_bIsRegularMode && m_ach_25ppl)
                    pPlayer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE,m_creature->GetEntry(),1,0,0,7168);

                if (m_bIsRegularMode && ach_spore_10)
                    pPlayer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE,m_creature->GetEntry(),1,0,0,7612);
                else if (!m_bIsRegularMode && ach_spore_25)
                    pPlayer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE,m_creature->GetEntry(),1,0,0,7613);
            }
        }
    }

    void CheckAch()
    {
        if (!m_pInstance)
            return;

        m_count_ppl = 0;
        Map::PlayerList const &PlList = m_pInstance->instance->GetPlayers();
        if (PlList.isEmpty())
            return;
        for(Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
        {
            if (Player* pPlayer = i->getSource())
            {
                if (pPlayer->isGameMaster())
                    continue;
                ++m_count_ppl;
            }
        }
        if (m_bIsRegularMode)
        {
            if(m_count_ppl>8)
                m_ach_10ppl = false;
        }
        else
        {
            if(m_count_ppl>20)
                m_ach_25ppl = false;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //NecroticAura_Timer
        if (NecroticAura_Timer < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_NECROTIC_AURA);
            NecroticAura_Timer = 20000;
        }else NecroticAura_Timer -= diff;

        //Deathbloom_Timer
        if (Deathbloom_Timer < diff)
        {
            DoCast(m_creature->getVictim(),!m_bIsRegularMode?SPELL_DEATHBLOOM_H:SPELL_DEATHBLOOM);
            if (m_pInstance && m_pInstance->GetData(ENCOUNT_HEIGAN) < DONE)
                Deathbloom_Timer = 18000;
            else
                Deathbloom_Timer = 25000;
        }else Deathbloom_Timer -= diff;

        //InevitableDoom_Timer
        if (InevitableDoom_Timer < diff)
        {
            DoCast(m_creature->getVictim(),!m_bIsRegularMode ? SPELL_INEVITABLE_DOOM_H:SPELL_INEVITABLE_DOOM);
            InevitableDoom_Timer = InevitableDoom_Cooldown;
			if (InevitableDoom_Cooldown > 15000)
				InevitableDoom_Cooldown -= 5000;
        }else InevitableDoom_Timer -= diff;

        //Summon_Timer
        if (Summon_Timer < diff)
        {
            Unit* target = NULL;
            Unit* SummonedSpores = NULL;
            switch (rand()%3)
            {
                case 0:
                    SummonedSpores = m_creature->SummonCreature(16286,ADD_1X,ADD_1Y,ADD_1Z,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000);
                    break;
                case 1:
                    SummonedSpores = m_creature->SummonCreature(16286,ADD_2X,ADD_2Y,ADD_2Z,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000);
					break;
                case 2:
                    SummonedSpores = m_creature->SummonCreature(16286,ADD_3X,ADD_3Y,ADD_3Z,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000);
					break;
			};
            if (SummonedSpores)
            {
				target = SelectUnit(SELECT_TARGET_RANDOM,0);
                if (target)
					SummonedSpores->AddThreat(target,1.0f);
            }
            Summon_Timer = 24000;
        } else Summon_Timer -= diff;

        if (Ach_Timer<diff)
        {
            if (m_bIsRegularMode && m_ach_10ppl)
                CheckAch();
            else if (!m_bIsRegularMode && m_ach_25ppl)
                CheckAch();
            Ach_Timer = 10000;
        }else Ach_Timer -= diff;  

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_loatheb(Creature *_Creature)
{
    return new boss_loathebAI (_Creature);
}

struct MANGOS_DLL_DECL mob_loatheb_sporesAI : public ScriptedAI
{
    mob_loatheb_sporesAI(Creature *c) : ScriptedAI(c)
    {
        m_pInstance = ((ScriptedInstance*)c->GetInstanceData());
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool InCombat;
    void Reset(){}

    void Aggro(Unit *who){}
    void JustDied(Unit* Killer)
    {
        DoCast(m_creature,SPELL_FUNGAL_CREEP,true);
        Creature* Loatheb = (Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_LOATHEB));
        if(Loatheb)
            ((boss_loathebAI*)Loatheb->AI())->KillSpore();

    }

    /*void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        if (m_creature->GetHealth()-uiDamage > m_creature->GetMaxHealth()*0.01)
            DoCast(m_creature,SPELL_FUNGAL_CREEP,true);
    }*/

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
}; 
CreatureAI* GetAI_mob_loatheb_spores(Creature *_Creature)
{
    return new mob_loatheb_sporesAI (_Creature);
}

void AddSC_boss_loatheb()
{
	Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_loatheb";
    newscript->GetAI = &GetAI_boss_loatheb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_loatheb_spores";
    newscript->GetAI = &GetAI_mob_loatheb_spores;
    newscript->RegisterSelf();
}