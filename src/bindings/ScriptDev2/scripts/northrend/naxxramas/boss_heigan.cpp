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
SDName: Boss_Heigan
SD%Complete: 0
SDComment: Place Holder
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

#define SAY_AGGRO1          -1533109
#define SAY_AGGRO2          -1533110
#define SAY_AGGRO3          -1533111
#define SAY_SLAY            -1533112
#define SAY_TAUNT1          -1533113
#define SAY_TAUNT2          -1533114
#define SAY_TAUNT3          -1533115
#define SAY_TAUNT4          -1533116
#define SAY_TAUNT5          -1533117
#define SAY_DEATH           -1533118

//Spell used by floor peices to cause damage to players
#define SPELL_ERUPTION      29371

//Spells by boss
#define SPELL_DISRUPTION    29310
#define SPELL_FEAVER        29998
#define SPELL_FEAVER_H      55011

//Spell by eye stalks
#define SPELL_PLAGUECLOUD   29350

struct MANGOS_DLL_DECL boss_heiganAI : public ScriptedAI
{
    
    boss_heiganAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }
    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_ach_10ppl;
    bool m_ach_25ppl;
    bool ach_nodied_10;
    bool ach_nodied_25;
    uint32 m_count_ppl;
    uint32 Ach_Timer;
    uint32 phase;
    uint32 safeSpot;
    uint32 fastTimer;
    uint32 phaseTimer;
    uint32 slowTimer;
    bool forward;
    uint32 diseaseTimer;
    uint32 disruptTimer;

    void Reset()
    {
        if(m_pInstance)
            m_pInstance->SetData(ENCOUNT_HEIGAN, NOT_STARTED);

        ach_nodied_10 = true;
        ach_nodied_25 = true;
        m_ach_10ppl = true;
        m_ach_25ppl = true;
        Ach_Timer = 10000;
        m_count_ppl = 0;

        phase = 1;
        safeSpot = 1;
        forward = true;
        fastTimer=7000;
        phaseTimer=90000;
        slowTimer=10500;
        diseaseTimer=20000;
        disruptTimer=urand(5000, 10000);
    }
    void JustDied(Unit* who)
    {
        if (!m_pInstance)
            return;
        DoScriptText(SAY_DEATH, m_creature);
        m_pInstance->SetData(ENCOUNT_HEIGAN, DONE);

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
                    pPlayer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE,m_creature->GetEntry(),1,0,0,7154);
                else if (!m_bIsRegularMode && m_ach_25ppl)
                    pPlayer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE,m_creature->GetEntry(),1,0,0,7167);

                if (m_bIsRegularMode && ach_nodied_10)
                    pPlayer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE,m_creature->GetEntry(),1,0,0,7264);
                else if (!m_bIsRegularMode && ach_nodied_25)
                    pPlayer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE,m_creature->GetEntry(),1,0,0,7548);
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
                if (pPlayer->isDead())
                {
                    ach_nodied_10 = false;
                    ach_nodied_25 = false;
                }
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

    void Aggro(Unit* who)
    {
        if(m_pInstance)
            m_pInstance->SetData(ENCOUNT_HEIGAN, IN_PROGRESS);
        CheckAch();
        switch(rand()%3)
        {
            case 0:            DoScriptText(SAY_AGGRO1, m_creature);            break;
            case 1:            DoScriptText(SAY_AGGRO2, m_creature);            break;
            case 2:            DoScriptText(SAY_AGGRO3, m_creature);            break;
        }
        m_creature->SetInCombatWithZone();
    }

    void KilledUnit(Unit *victim)
    {
        DoScriptText(SAY_SLAY, m_creature);
    }

    
    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Ach_Timer<diff)
        {
            CheckAch();
            Ach_Timer = 8000;
        }else Ach_Timer -= diff;

        //Don't let Heigan walk thru the door
        if (m_creature->GetPositionX()>2822 && m_creature->GetPositionY()>(-3700))
        {
            EnterEvadeMode();
            return;
        }
        if (m_creature->GetPositionY()<(-3739))
        {
            EnterEvadeMode();
            return;
        }
        //Slow Dance
        if (phase == 1)
        {
            if(phaseTimer < diff)
            {
                //Let's start fast dance
                phase = 2;
                //Teleport To Start Position
                //m_creature->NearTeleportTo(2793.86, -3707.38, 276.627, 0.593412);
                m_creature->GetMap()->CreatureRelocation(m_creature, 2793.86, -3707.38, 276.627, 0.593412);
                m_creature->SendMonsterMove(2793.86, -3707.38, 276.627, 0, MONSTER_MOVE_NONE, 0);
                switch (rand()%5)
                {
                    case 0: DoScriptText(SAY_TAUNT1, m_creature); break;
                    case 1: DoScriptText(SAY_TAUNT2, m_creature); break;
                    case 2: DoScriptText(SAY_TAUNT3, m_creature); break;
                    case 3: DoScriptText(SAY_TAUNT4, m_creature); break;
                    case 4: DoScriptText(SAY_TAUNT5, m_creature); break;
                }
                //Cast channeling AOE
                DoCast(m_creature, SPELL_PLAGUECLOUD);
                phaseTimer = 45000;
                safeSpot = 1;
            }else phaseTimer-=diff;
            //Disease  all nearby players
            if(diseaseTimer < diff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_FEAVER_H : SPELL_FEAVER);
                diseaseTimer=20000;
            }else diseaseTimer-=diff;
            //Debuff all players in 20 yard radius to 300% cast time
            if(disruptTimer < diff)
            {
                DoCast(m_creature, SPELL_DISRUPTION, false);
                disruptTimer = urand(5000, 10000);
            }else disruptTimer-=diff;

            DoMeleeAttackIfReady();
            
        }
        else if(phase == 2)
        {
            if(phaseTimer < diff)
            {
                switch (rand()%5)
                {
                    case 0: DoScriptText(SAY_TAUNT1, m_creature); break;
                    case 1: DoScriptText(SAY_TAUNT2, m_creature); break;
                    case 2: DoScriptText(SAY_TAUNT3, m_creature); break;
                    case 3: DoScriptText(SAY_TAUNT4, m_creature); break;
                    case 4: DoScriptText(SAY_TAUNT5, m_creature); break;
                }
                phase = 1;
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                AttackStart(m_creature->getVictim());
                phaseTimer = 90000;
                safeSpot = 1;
            }else phaseTimer-=diff;   
        }
    }
};

struct MANGOS_DLL_DECL npc_heigan_eruptionAI : public ScriptedAI
{
    npc_heigan_eruptionAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }
    ScriptedInstance* pInstance;

    uint32 phase;
    uint32 safeSpot;
    uint32 fastTimer;
    uint32 phaseTimer;
    uint32 slowTimer;
    uint64 heiganGUID;
    bool forward;
    std::list<GameObject*> GetGameObjectsByEntry(uint32 entry)
    {
        CellPair pair(MaNGOS::ComputeCellPair(m_creature->GetPositionX(), m_creature->GetPositionY()));
        Cell cell(pair);
        cell.data.Part.reserved = ALL_DISTRICT;
        cell.SetNoCreate();

        std::list<GameObject*> gameobjectList;

        AllGameObjectsWithEntryInRange check(m_creature, entry, 100);
        MaNGOS::GameObjectListSearcher<AllGameObjectsWithEntryInRange> searcher(m_creature, gameobjectList, check);
        TypeContainerVisitor<MaNGOS::GameObjectListSearcher<AllGameObjectsWithEntryInRange>, GridTypeMapContainer> visitor(searcher);

        CellLock<GridReadGuard> cell_lock(cell, pair);
        cell_lock->Visit(cell_lock, visitor, *(m_creature->GetMap()));

        return gameobjectList;
    }
    //Let's Dance!
    void DoErupt(uint32 safePlace)
    {
        if(!pInstance)
            return;

        heiganGUID = pInstance->GetData64(GUID_HEIGAN);
        Map::PlayerList const &PlList = pInstance->instance->GetPlayers();
        if (PlList.isEmpty())
            return;

        if(safePlace != 1)
        {
            std::list<GameObject*> eruptGOs = GetGameObjectsByEntry(181678);
            //Visual part of eruption
            for (int32 i = 181510; i <= 181526; i++)
            {
                if (i == 181513 || i == 181512 || i == 181511 || i == 181525 || i == 181514 || i == 181515 || i == 181516)
                    continue;
                
                std::list<GameObject*> visualGO = GetGameObjectsByEntry(i);
                for (std::list<GameObject*>::iterator itr = visualGO.begin(); itr != visualGO.end(); ++itr)
                {
                    if((*itr))
                        //Required GO Custom Animation Patch for this
                    {
                        WorldPacket data(SMSG_GAMEOBJECT_CUSTOM_ANIM,8+4);
                        data << (*itr)->GetGUID();
                        data << 0;
                        (*itr)->SendMessageToSet(&data,true);
                    }
                }
            }
            //Damage part of eruption
            for (std::list<GameObject*>::iterator itr = eruptGOs.begin(); itr != eruptGOs.end(); ++itr)
            {
                if(!(*itr))
                    continue;
                for(Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                {
                    if (Player* pPlayer = i->getSource())
                    {
                        if (pPlayer->isGameMaster())
                            continue;

                        if (pPlayer->isAlive())
                        {
                            if(pPlayer->GetDistance((*itr)) <= 6.0f)
                                //We use originalCaster for deal damage by Plague Fissure
                                DoCast(pPlayer, SPELL_ERUPTION, true);
                        }
                    }
                }
            }
        }
        //Change direction of dance
        else forward = true;
        if(safePlace != 2)
        {
            std::list<GameObject*> eruptGOs = GetGameObjectsByEntry(181676);
            for (int32 i = 181511; i <= 181531; i++)
            {
                if ((i > 181516 && i < 181525) || (i == 181526))
                    continue;
                std::list<GameObject*> visualGO = GetGameObjectsByEntry(i);
                for (std::list<GameObject*>::iterator itr = visualGO.begin(); itr != visualGO.end(); ++itr)
                {
                    if((*itr))
                    {
                        WorldPacket data(SMSG_GAMEOBJECT_CUSTOM_ANIM,8+4);
                        data << (*itr)->GetGUID();
                        data << 0;
                        (*itr)->SendMessageToSet(&data,true);
                    }
                }
            }
            for (std::list<GameObject*>::iterator itr = eruptGOs.begin(); itr != eruptGOs.end(); ++itr)
            {
                if(!(*itr))
                    continue;
                for(Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                {
                    if (Player* pPlayer = i->getSource())
                    {
                        if (pPlayer->isGameMaster())
                            continue;

                        if (pPlayer->isAlive())
                        {
                            if(pPlayer->GetDistance((*itr)) <= 6.0f)
                                DoCast(pPlayer, SPELL_ERUPTION, true);
                        }
                    }
                }
            }
        }
        if(safePlace != 3)
        {
            std::list<GameObject*> eruptGOs = GetGameObjectsByEntry(181677);
            for (int32 i = 181532; i <= 181545; i++)
            {
                if (i >= 181537 && i <= 181539)
                    continue;
                std::list<GameObject*> visualGO = GetGameObjectsByEntry(i);
                for (std::list<GameObject*>::iterator itr = visualGO.begin(); itr != visualGO.end(); ++itr)
                {
                    if((*itr))
                    {
                        WorldPacket data(SMSG_GAMEOBJECT_CUSTOM_ANIM,8+4);
                        data << (*itr)->GetGUID();
                        data << 0;
                        (*itr)->SendMessageToSet(&data,true);
                    }
                }
            }
            for (std::list<GameObject*>::iterator itr = eruptGOs.begin(); itr != eruptGOs.end(); ++itr)
            {
                if(!(*itr))
                    continue;
                for(Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                {
                    if (Player* pPlayer = i->getSource())
                    {
                        if (pPlayer->isGameMaster())
                            continue;

                        if (pPlayer->isAlive())
                        {
                            if(pPlayer->GetDistance((*itr)) <= 6.0f)
                                DoCast(pPlayer, SPELL_ERUPTION, true);
                        }
                    }
                }
            }
        }
        if(safePlace != 4)
        {
            std::list<GameObject*> eruptGOs = GetGameObjectsByEntry(181695);
            for (int32 i = 181537; i <= 181552; i++)
            {
                if (i > 181539 && i < 181545)
                    continue;
                std::list<GameObject*> visualGO = GetGameObjectsByEntry(i);
                for (std::list<GameObject*>::iterator itr = visualGO.begin(); itr != visualGO.end(); ++itr)
                {
                    if((*itr))
                    {
                        WorldPacket data(SMSG_GAMEOBJECT_CUSTOM_ANIM,8+4);
                        data << (*itr)->GetGUID();
                        data << 0;
                        (*itr)->SendMessageToSet(&data,true);
                    }
                }
            }
            for (std::list<GameObject*>::iterator itr = eruptGOs.begin(); itr != eruptGOs.end(); ++itr)
            {
                if(!(*itr))
                    continue;
                for(Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                {
                    if (Player* pPlayer = i->getSource())
                    {
                        if (pPlayer->isGameMaster())
                            continue;

                        if (pPlayer->isAlive())
                        {
                            if(pPlayer->GetDistance((*itr)) <= 6.0f)
                                DoCast(pPlayer, SPELL_ERUPTION, true);
                        }
                    }
                }
            }
        //Let's dance back!
        }else forward=false;
    }

    void Reset()
    {
        heiganGUID;
        phase = 1;
        safeSpot = 1;
        fastTimer = 7000;
        slowTimer = 10500;
        phaseTimer = 90000;
    }
    void Aggro(Unit* who)
    {
        //This is just for dance. It doesn't attack anybody.
        DoStopAttack();
        SetCombatMovement(false);
    }
    void JustDied(Unit* who)
    {
        //If dance mob was somehow killed - respawn him.
        m_creature->Respawn();
    }
    void UpdateAI(const uint32 diff)
    {
        if(pInstance && pInstance->GetData(ENCOUNT_HEIGAN) != IN_PROGRESS)
        {
            //We dance only when fighting Heigan
            EnterEvadeMode();
            return;
        }

        if (phase == 1)
        {
            if(phaseTimer < diff)
            {
                // Let's fast dance
                phase = 2;
                phaseTimer = 45000;
                safeSpot = 1;
            }else phaseTimer-=diff;
            if(slowTimer < diff)
            {
                DoErupt(safeSpot);
                if(forward)
                    safeSpot++;
                else
                    safeSpot--;
                slowTimer = 10500;
            }else slowTimer-=diff;
        }
        else if(phase == 2)
        {
            if(phaseTimer < diff)
            {
                // Slow dance again
                phase = 1;
                AttackStart(m_creature->getVictim());
                phaseTimer = 90000;
                safeSpot = 1;
            }else phaseTimer-=diff;   
            if(fastTimer < diff)
            {
                DoErupt(safeSpot);
                if(forward)
                    safeSpot++;
                else
                    safeSpot--;
                fastTimer = 3500;
            }else fastTimer-=diff;
        }
    }
};
CreatureAI* GetAI_boss_heiganAI(Creature* pCreature)
{
    return new boss_heiganAI(pCreature);
}
CreatureAI* GetAI_npc_heigan_eruptionAI(Creature* pCreature)
{
    return new npc_heigan_eruptionAI(pCreature);
}
void AddSC_boss_heigan()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_heigan";
    newscript->GetAI = &GetAI_boss_heiganAI;
    newscript->RegisterSelf();
}

void AddSC_npc_heigan_eruption()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "npc_heigan_eruption";
    newscript->GetAI = &GetAI_npc_heigan_eruptionAI;
    newscript->RegisterSelf();
}